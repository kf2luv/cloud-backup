#include "util.hpp"
#include <iostream>
#include <ctime>
#include "json/json.h"
// #include "config.hpp"

void fileUtilTest(const std::string& path)
{    
    Util::FileUtil file(path);
    std::cout << file.fileSize() << std::endl;

    time_t at = file.lastAccessTime();
    time_t mt = file.lastModTime();
    std::cout << "最近访问时间" << std::ctime(&at) << std::endl;
    std::cout << "最近修改时间" << std::ctime(&mt) << std::endl;

    std::cout << file.fileName() << std::endl;

    // std::string input1;
    // if (file.getContent(input1))
    //     std::cout << input1 << std::endl;
    
    // std::string input2;
    // if(file.getPosLen(input2, 3, 5));
    //     std::cout << input2 << std::endl;

    // std::string output("Hello World!");
    // file.setContent(output);

    // file.compress("bundle.cpp.lz");

    // Cloud::FileUtil file2("bundle.cpp.lz");
    // file.uncompress("bundle.cpp");

    Util::FileUtil fu("mydir");
    if(!fu.isExists())
    {
        fu.createDirectory();
    }

    std::vector<std::string> array;
    fu.scanDirectory(array);
    for(auto p : array)
    {
        std::cout << p << std::endl;
    }
}

void jsonTest()
{
    Json::Value value1;
    value1["name"] = "Job";
    value1["age"] = 18;
    value1["salary"] = 8800.88;

    std::string str1;
    Util::JsonUtil::serialize(value1, &str1);
    std::cout << str1 << std::endl;

    Json::Value value2;
    Util::JsonUtil::unserialize(str1, &value2);
    if(value1 == value2)
    {
        std::cout << "反序列化成功！" << std::endl;
    }
}

// void configTest()
// {
//     Cloud::Config *conf = Cloud::Config::getInstance();
//     std::cout << conf->getPackDir() << std::endl;
//     std::cout << conf->getArcSuffix() << std::endl;
//     std::cout << conf->getSvrIP() << std::endl;
//     std::cout << conf->getSvrPort() << std::endl;
//     std::cout << conf->getFileDir() << std::endl;
//     std::cout << conf->getHotTime() << std::endl;
//     std::cout << conf->getUrlPrefix() << std::endl;
//     std::cout << conf->getManagerFile() << std::endl;
// }

int main(int argc, char* argv[])
{
    fileUtilTest(argv[1]);
    return 0;
}