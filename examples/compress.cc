#include <iostream>
#include <fstream>
#include <memory.h>
#include "bundle/bundle.h"

void usage()
{
    std::cout << "-------- USAGE --------" << std::endl;
    std::cout << "./compress path target" << std::endl;
}

int main(int argc, char* argv[])
{
    //argv
    //第一个参数：将要压缩的文件路径
    //第二个参数：压缩包名称
    std::string path(argv[1]);
    std::string target(argv[2]);
    if(argc != 3){
        usage();
        return -1;
    }
    std::cout << "将要压缩的文件路径" << path << std::endl;
    std::cout << "压缩包名称" << target << std::endl;

    //1.打开path, 将文件内容整理为string
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ios::binary);
    if (!ifs) { // 检查文件是否成功打开
        std::cerr << "无法打开文件" <<  path << std::endl;
        return 1;
    }

    //获取文件大小
    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    //组织为string
    std::string input;
    input.resize(length);
    ifs.read(&input[0], length);
    ifs.close();
    
    std::cout << "压缩前的字符串大小" << input.size() << std::endl;
    //2.使用bundle压缩
    std::string output = bundle::pack(bundle::LZIP, input);
    std::cout << "压缩后的字符串大小" << output.size() << std::endl;

    //3.将output写入到压缩包中
    std::ofstream ofs(target.c_str(), std::ios::binary);
    ofs.write(output.c_str(), output.size());
    if (!ofs) { // 检查文件是否成功打开
        std::cerr << "无法生成文件" << target << std::endl;
        return 1;
    }
    ofs.close();

    std::cout << "压缩成功！"  << std::endl;

    return 0;
}