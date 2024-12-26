#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"

Cloud::BackupInfoManager *_biManager;

// void dataTest()
// {
//     // Cloud::BackupInfo fdata(argv[1]);

//     // std::cout << fdata.fsize << std::endl;
//     // std::cout << fdata.pack_flag << std::endl;
//     // std::cout << fdata.atime << std::endl;
//     // std::cout << fdata.mtime << std::endl;
//     // std::cout << fdata.real_path << std::endl;
//     // std::cout << fdata.pack_path << std::endl;
//     // std::cout << fdata.url << std::endl;

//     _biManager->insert("/download/a.txt", Cloud::BackupInfo("./a.txt"));
//     _biManager->insert("/download/b.txt", Cloud::BackupInfo("./b.txt"));

//     // Cloud::BackupInfoManager fdm;
//     // std::vector<Cloud::BackupInfo> arr;
//     // fdm.getAll(&arr);
//     // for (auto fdata : arr)
//     // {
//     //     std::cout << fdata.fsize << std::endl;
//     //     std::cout << fdata.pack_flag << std::endl;
//     //     std::cout << fdata.atime << std::endl;
//     //     std::cout << fdata.mtime << std::endl;
//     //     std::cout << fdata.real_path << std::endl;
//     //     std::cout << fdata.pack_path << std::endl;
//     //     std::cout << fdata.url << std::endl;
//     //     std::cout << "----------------------------------" << std::endl;
//     // }
// }


void hotTest2()
{
    Cloud::HotManager hm;
    hm.run();    
}

void serviceTest()
{
    Cloud::Service service;
    service.run();
}

int main(int argc, char *argv[])
{
    _biManager = new Cloud::BackupInfoManager;
    // hotTest2();
    serviceTest();
    return 0;
}