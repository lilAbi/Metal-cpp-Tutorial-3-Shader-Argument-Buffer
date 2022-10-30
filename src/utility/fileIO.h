
#ifndef IRONENGINE_FILEIO_H
#define IRONENGINE_FILEIO_H
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

namespace util{
    std::string ReadFileIntoString(const std::string& fileName);
}


#endif //IRONENGINE_FILEIO_H
