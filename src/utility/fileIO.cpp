#include "fileIO.h"

std::string util::ReadFileIntoString(const std::string& fileName){
    std::string data;

    std::fstream shaderFile(fileName, std::ios_base::in);

    if(shaderFile){
        shaderFile.seekg(0, std::ios::end);
        data.resize(shaderFile.tellg());
        shaderFile.seekg(0, std::ios::beg);
        shaderFile.read(&data[0], (long)data.size());
    }
    shaderFile.close();

    return data;
}
