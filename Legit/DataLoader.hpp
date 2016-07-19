#pragma once

#include <vector>
#include <string>

namespace Legit
{

namespace DataLoader
{
    #ifdef _WIN32
    bool LoadFromResource(std::wstring name, std::vector<char> &data);
    #endif
    bool LoadFromFile(std::wstring path, std::vector<char> &data);
    bool DumpToFile(std::wstring path, std::vector<char> &data);
}

} // end namespace
