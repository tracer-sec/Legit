#pragma once

#include <string>
#include <vector>

namespace Legit
{

namespace FileSystem
{
    std::vector<std::wstring> GetSubdirectories(std::wstring path);
    std::vector<std::wstring> GetFiles(std::wstring path);
    std::vector<std::wstring> ExpandWildcards(std::wstring pattern);
    bool FileExists(std::wstring path);
    bool DirectoryExists(std::wstring path);
    std::wstring GetTempPath();
}

} // end namespace

