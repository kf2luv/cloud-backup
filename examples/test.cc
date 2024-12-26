#include "json/json.h"
#include <iostream>
#include <sstream>
#include <memory>

int main()
{
    // 保存数据到json::value对象中
    // Json::Value value;
    // value["姓名"] = "卡尔";
    // value["年龄"] = 18;

    // float score[3] = {34.5, 80, 91};
    // value["分数"].append(score[0]);
    // value["分数"].append(score[1]);
    // value["分数"].append(score[2]);

    // // 序列化
    // Json::StreamWriterBuilder swb;//搞一个builder
    // swb["emitUTF8"] = true;

    // std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());//由builder构建一个writer

    // std::ostringstream oss;
    // writer->write(value, &oss);
    // std::string str = oss.str();

    // std::cout << jsonStr << std::endl;

    // 反序列化
    // Json::CharReaderBuilder crb;
    // Json::Value value2;
    // std::unique_ptr<Json::CharReader> reader(crb.newCharReader());
    // std::string err;
    // bool ok = reader->parse(str.c_str(), str.c_str() + str.size(), &value2, &err);
    // if(!ok){
    //     std::cout << "解析失败: " << err << std::endl;
    //     return -1;
    // }
    // std::cout << "解析成功: " << std::endl;
    // std::cout << value2["姓名"].asString() << std::endl;//转化成中文
    // std::cout << value2["年龄"] << std::endl;
    // std::cout << value2["分数"][0] << std::endl;
    // std::cout << value2["分数"][1] << std::endl;
    // std::cout << value2["分数"][2] << std::endl;

    std::string str = R"({"姓名":  "mike", "年龄": 18, "分数": 90.5})";
    Json::Value root;
    Json::CharReaderBuilder crBuilder;
    std::unique_ptr<Json::CharReader> reader(crBuilder.newCharReader());
    std::string err_str;
    bool ok = reader->parse(str.c_str(), str.c_str() + str.size(), &root, &err_str);
    if(!ok){
        std::cout << "解析失败: " << err_str << std::endl;
        return -1;
    }
    std::cout << "解析成功: " << std::endl;
    std::cout << root["姓名"].asString() << std::endl;
    std::cout << root["年龄"].asInt() << std::endl;
    std::cout << root["分数"].asFloat() << std::endl;

    return 0;
}
