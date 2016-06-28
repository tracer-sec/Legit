#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Legit
{

struct HttpFile
{
    HttpFile(std::string f, std::vector<char> c) :
         filename(f), file(c)
    { }

    std::string filename;
    std::vector<char> file;
};

namespace HttpUtils
{

// application/x-www-form-urlencoded
std::vector<char> CreateBody(std::unordered_map<std::string, std::string> fields);
// multipart/form-data
std::vector<char> CreateBody(std::unordered_map<std::string, std::string> fields, std::unordered_map<std::string, HttpFile> files, std::string &boundary);

}
    
} // end namespace
