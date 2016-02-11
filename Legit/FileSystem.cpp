#include "FileSystem.h"

#include <Windows.h>
#include <Shlwapi.h>

using namespace std;

vector<wstring> FileSystem::GetSubdirectories(wstring path)
{
    vector<wstring> result;
    WIN32_FIND_DATA findData;

    if (path[path.length() - 1] != L'\\')
        path = path + L'\\';
    path = path + L"*.*";

    HANDLE handle = ::FindFirstFile(path.c_str(), &findData);
    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            wstring fuckYourCStrings(findData.cFileName);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                && fuckYourCStrings != L"."
                && fuckYourCStrings != L".."
                )
            {
                // recombine with path
                result.push_back(wstring(findData.cFileName));
            }
        } while (::FindNextFile(handle, &findData));
    }

    ::FindClose(handle);

    return result;
}

vector<wstring> FileSystem::GetFiles(wstring path)
{
    vector<wstring> result;
    WIN32_FIND_DATA findData;
    wchar_t buffer[MAX_PATH];
    wstring directory = path.substr(0, path.rfind(L"\\"));

    HANDLE handle = ::FindFirstFile(path.c_str(), &findData);
    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // recombine with path
                ::PathCombine(buffer, directory.c_str(), findData.cFileName);
                result.push_back(wstring(buffer));
            }
        } while (::FindNextFile(handle, &findData));
    }

    ::FindClose(handle);

    return result;
}

vector<wstring> FileSystem::ExpandWildcards(wstring pattern)
{
    vector<wstring> result;
    auto wildcard = pattern.find(L"*\\");
    if (wildcard != wstring::npos)
    {
        vector<wstring> w = GetSubdirectories(pattern.substr(0, wildcard + 1));
        for (auto path : w)
        {
            // Filter out anything not a directory
            wstring newPath = pattern.substr(0, wildcard) + path + pattern.substr(wildcard + 1);
            vector<wstring> gottaGoDeeper = ExpandWildcards(newPath);
            for (auto p : gottaGoDeeper)
                result.push_back(p);
        }
    }
    else
    {
        // Simple
        result.push_back(pattern);
    }
    return result;
}

bool FileSystem::FileExists(wstring path)
{
    DWORD fileData = ::GetFileAttributes(path.c_str());
    return (fileData != INVALID_FILE_ATTRIBUTES && !(fileData & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileSystem::DirectoryExists(wstring path)
{
    DWORD fileData = ::GetFileAttributes(path.c_str());
    return (fileData != INVALID_FILE_ATTRIBUTES && (fileData & FILE_ATTRIBUTE_DIRECTORY));
}


