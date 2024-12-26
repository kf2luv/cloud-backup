#pragma once
#include <iostream>
#include "httplib.h"
#include "config.hpp"
#include "data.hpp"
#include "log/ckflog.hpp"

extern Cloud::BackupInfoManager *_biManager;
extern ckflogs::Logger::Ptr _logger;

namespace Cloud
{
    class Service
    {
    public:
        Service();
        void run();

    private:
        static void index(const httplib::Request &req, httplib::Response &resp);      // 起始界面
        static void login(const httplib::Request &req, httplib::Response &resp);      // 用户登录
        static void upload(const httplib::Request &req, httplib::Response &resp);     // 文件上传
        static void download(const httplib::Request &req, httplib::Response &resp);   // 文件下载
        static void listShow(const httplib::Request &req, httplib::Response &resp);   // 文件列表展示
        static void uploadShow(const httplib::Request &req, httplib::Response &resp); // 上传页面展示
        static void updateList(const httplib::Request &req, httplib::Response &resp); // 前端更新文件列表

        static std::string getETag(const std::string &url);

    private:
        int _svr_port;        // 端口号
        std::string _svr_ip;  // 服务端ip
        httplib::Server _svr; // 服务器
    };
}

Cloud::Service::Service()
{
    Config *conf = Config::getInstance();
    _svr_port = conf->getSvrPort();
    _svr_ip = conf->getSvrIP();
}

void Cloud::Service::run()
{
    _svr.Get("/", index);                // 起始界面
    _svr.Post("/login", login);          // 用户登录
    _svr.Post("/upload", upload);        // 文件上传
    _svr.Get("/uploadShow", uploadShow); // 文件上传展示页面
    _svr.Get("/download/.*", download);  // 文件下载
    _svr.Get("/file-list", updateList); // 文件列表展示
    _svr.Get("/list", listShow);        // 前端页面更新文件列表

    if (!_svr.listen("0.0.0.0", _svr_port))
    {
        _logger->_fatal("服务器监听失败 %s", strerror(errno));
        exit(-2);
    }
}

void Cloud::Service::index(const httplib::Request &req, httplib::Response &resp)
{
    resp.set_file_content("../www/index.html");
    resp.set_header("Content-Type", "text/html");
    resp.status = 200;
}

void Cloud::Service::login(const httplib::Request &req, httplib::Response &resp)
{
    // 获取用户名和密码
    Json::Value root;
    if(!Util::JsonUtil::unserialize(req.body, &root)){
        return;
    }
    std::string username = root["username"].asCString();
    std::string password = root["password"].asCString();

    // 查看用户数据库，查看[用户名-密码]是否合法
    if (Util::checkUser(username, password))
    {
        // 重定向到文件列表页面
        resp.set_header("Location", "/list"); 
        resp.status = 302;
    }
    else
    {
        // 用户验证失败，返回错误信息
        resp.status = 401;
        resp.set_content("Invalid username or password", "text/plain");
    }
}

void Cloud::Service::upload(const httplib::Request &req, httplib::Response &resp)
{
    // 处理请求
    // 1.获取上传的文件内容  (一次只传一个文件)
    if (!req.has_file("file"))
    {
        resp.status = 400;
        resp.set_content("File not exists", "text/plain");
        return;
    }
    auto mfd = req.get_file_value("file"); // 获取一个文件内容

    // 2.添加新文件
    std::string real_path = Config::getInstance()->getBackupDir() + mfd.filename;
    Util::FileUtil fu(real_path);
    fu.setContent(mfd.content);

    // 3.添加备份信息
    BackupInfo newbi(real_path);
    _biManager->update(newbi.url, newbi);

    // 返回响应
    resp.status = 200;
    resp.set_content("Upload successful", "text/plain");

    _logger->_debug("用户上传文件已存入: %s", real_path.c_str());
}

void Cloud::Service::download(const httplib::Request &req, httplib::Response &resp)
{
    // 1.ETag缓存判断机制
    if (req.has_header("If-None-Match"))
    {
        std::string old_etag = req.get_header_value("If-None-Match");
        if (old_etag == getETag(req.path))
        {
            resp.status = 304;
            resp.reason = "Not Modified";
            return;
        }
    }

    // 2.以URL查找文件
    BackupInfo bi;
    if (!_biManager->getOneByURL(req.path, &bi))
    {
        // 文件不存在
        resp.status = 404;
        resp.set_content("File not found", "text/plain");
        return;
    }

    // 3.判断文件是否为热点文件，若不是，需要先解压
    // 若文件正在压缩中，需要等待其压缩结束，再解压

    if (bi.pack_flag == true)
    {
        // 非热点文件 -> 热点文件
        Util::FileUtil fu(bi.pack_path);
        fu.uncompress(bi.real_path);
        bi.pack_flag = false;
        fu.remove();
        _biManager->update(bi.url, bi);

        _logger->_debug("热点文件: %s 处理成功", bi.real_path.c_str());
    }

    // 判断是否为断点续传(断点下载)请求
    if (req.has_header("If-Range"))
    {
        // 判断客户端的etag是否与当前etag相等
        // 若不相等，表示服务端对文件进行了修改，客户端不能进行断点续传，需要重新下载
        std::string old_etag = req.get_header_value("If-Range");
        if (old_etag == getETag(req.path))
        {
            Util::FileUtil fu(bi.real_path);
            fu.getContent(resp.body); // cpp-httplib库内置处理断点续传

            resp.set_header("ETag", getETag(req.path));
            resp.set_header("Accept-Ranges", "bytes");
            resp.status = 206;
            resp.reason = "Partial Content";
            return;
        }
    }

    // 4.获取文件内容，填充响应
    resp.set_file_content(bi.real_path);
    // 设置 Content-Disposition 以便下载文件而不是直接在浏览器显示
    std::string filename = Util::FileUtil(bi.real_path).fileName();
    resp.set_header("Content-Disposition", "attachment; filename=" + filename);
    // 设置ETag
    resp.set_header("ETag", getETag(req.path));
    // 设置接受断点续传
    resp.set_header("Accept-Ranges", "bytes");
    // 设置状态码和状态描述
    resp.status = 200;
    resp.reason = "OK";
}

void Cloud::Service::listShow(const httplib::Request &req, httplib::Response &resp)
{
    resp.set_file_content("../www/list.html");
    resp.set_header("Content-Type", "text/html");
    resp.status = 200;
}

void Cloud::Service::uploadShow(const httplib::Request &req, httplib::Response &resp)
{
    resp.set_file_content("../www/upload.html");
    resp.set_header("Content-Type", "text/html");
    resp.status = 200;
}

void Cloud::Service::updateList(const httplib::Request &req, httplib::Response &resp)
{
    // 1.获取可下载的文件列表(热点 or 非热点都可下载)
    std::vector<Cloud::BackupInfo> list;
    _biManager->getAll(&list);

    Json::Value root;

    auto time_tToDateString = [](time_t time)
    {
        struct tm *timeinfo = std::localtime(&time);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    };

    auto size_tToString = [](size_t sz)
    {
        static const size_t B = 1; // 1字节
        static const size_t K = 1024 * B;
        static const size_t M = 1024 * K;
        static const size_t G = 1024 * M;

        if (sz < K)
            return std::to_string(sz) + "B";
        else if (sz >= K && sz < M)
            return std::to_string(sz / K) + "KB";
        else if (sz >= M && sz < G)
            return std::to_string(sz / M) + "MB";
        else
            return std::to_string(sz / G) + "GB";
    };

    for (auto &info : list)
    {
        Json::Value item;
        Config *conf = Config::getInstance();
        item["downloadUrl"] = info.url;
        item["fileName"] = info.url.substr(conf->getUrlPrefix().size());
        item["lastModified"] = time_tToDateString(info.mtime);
        item["fileSize"] = size_tToString(info.fsize);
        root.append(item);
    }

    std::string jsonStr;
    Util::JsonUtil::serialize(root, &jsonStr);
    resp.set_content(jsonStr, "application/json");
}

std::string Cloud::Service::getETag(const std::string &url)
{
    Cloud::BackupInfo bi;
    _biManager->getOneByURL(url, &bi);

    // 文件名-文件大小-最近修改时间
    std::string fileName = Util::FileUtil(bi.real_path).fileName();
    std::string fileSize = std::to_string(bi.fsize);
    std::string lastMTime = std::to_string(bi.mtime);
    return fileName + '-' + fileSize + '-' + lastMTime;
}
