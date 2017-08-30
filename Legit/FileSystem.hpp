#pragma once

#include <string>
#include <vector>

#undef GetTempPath // Fuck you, Win32

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
    bool SetFileDates(std::wstring path, time_t createdDate, time_t modifiedDate, time_t accessedDate);
}

} // end namespace

