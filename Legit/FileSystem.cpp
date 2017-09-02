#include "Utils.hpp"
#include "Environment.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <Shlwapi.h>
#else
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/time.h>
#endif

// Unconventional include order, because Win32 loves its stupid macro bullshit
#include "FileSystem.hpp"

using namespace Legit;
using namespace std;

vector<wstring> FileSystem::GetSubdirectories(wstring path)
{
    vector<wstring> result;
    #ifdef _WIN32
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
    #else
    string p = Utils::StringFromWide(path);
    DIR *directory = ::opendir(p.c_str());
    if (directory)
    {
        dirent *entry = ::readdir(directory);
        while (entry != nullptr)
        {
            string filename(entry->d_name);
            if (entry->d_type == DT_DIR
                && filename != "." && filename != "..")
            {
                result.push_back(Utils::WideFromString(filename));
            }
            entry = ::readdir(directory);
        }
        ::closedir(directory);
    }
    #endif
    return result;
}

vector<wstring> FileSystem::GetFiles(wstring path)
{
    vector<wstring> result;
    #ifdef _WIN32
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
    #else
    string p = Utils::StringFromWide(path);
    DIR *directory = ::opendir(p.c_str());
    if (directory)
    {
        dirent *entry = ::readdir(directory);
        while (entry != nullptr)
        {
            if (entry->d_type == DT_REG || entry->d_type == DT_LNK)
            {
                string filename(entry->d_name);
                result.push_back(Utils::WideFromString(filename));
            }
            entry = ::readdir(directory);
        }
        ::closedir(directory);
    }
    #endif
    return result;
}

vector<wstring> FileSystem::ExpandWildcards(wstring pattern)
{
    vector<wstring> result;

    auto wildcard = pattern.find(wstring(L"*") + Legit::SEPARATOR_WIDE);
    if (wildcard != wstring::npos)
    {
        vector<wstring> w = GetSubdirectories(pattern.substr(0, wildcard));
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
    #ifdef _WIN32
    DWORD fileData = ::GetFileAttributes(path.c_str());
    return (fileData != INVALID_FILE_ATTRIBUTES && !(fileData & FILE_ATTRIBUTE_DIRECTORY));
    #else
    return access(Utils::StringFromWide(path).c_str(), F_OK) != -1;
    #endif
}

bool FileSystem::DirectoryExists(wstring path)
{
    #ifdef _WIN32
    DWORD fileData = ::GetFileAttributes(path.c_str());
    return (fileData != INVALID_FILE_ATTRIBUTES && (fileData & FILE_ATTRIBUTE_DIRECTORY));
    #else
    return access(Utils::StringFromWide(path).c_str(), F_OK) != -1;
    #endif
}

wstring FileSystem::GetTempPath()
{
    #ifdef _WIN32
    DWORD bufferLength = MAX_PATH;
    TCHAR path[MAX_PATH];
    ::GetTempPathW(bufferLength, path);
    return wstring(path);
    #else
    return L"/tmp";
    #endif
}

#ifdef _WIN32

// Stolen from MSDN
// https://msdn.microsoft.com/library/windows/desktop/ms724228%28v=vs.85%29.aspx
FILETIME ConvertToFileTime(time_t t)
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    FILETIME result;
    result.dwLowDateTime = (DWORD)ll;
    result.dwHighDateTime = ll >> 32;
    return result;
}

#endif

bool FileSystem::SetFileDates(wstring path, time_t createdDate, time_t modifiedDate, time_t accessedDate)
{
    bool success = false;

    #ifdef _WIN32
    HANDLE fileHandle = ::CreateFile(path.c_str(), FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        FILETIME *creationTimePtr = nullptr;
        FILETIME creationTime = ConvertToFileTime(createdDate);
        FILETIME *accessedTimePtr = nullptr;
        FILETIME accessedTime = ConvertToFileTime(accessedDate);
        FILETIME *modifiedTimePtr = nullptr;
        FILETIME modifiedTime = ConvertToFileTime(modifiedDate);

        if (createdDate > 0)
            creationTimePtr = &creationTime;
        if (accessedDate > 0)
            accessedTimePtr = &accessedTime;
        if (modifiedDate > 0)
            modifiedTimePtr = &modifiedTime;

        success = ::SetFileTime(fileHandle, creationTimePtr, accessedTimePtr, modifiedTimePtr) == TRUE;
        ::CloseHandle(fileHandle);
    }
    #else
    timeval t[2];
    t[0].tv_sec = accessedDate;
    t[0].tv_usec = 0;
    t[1].tv_sec = modifiedDate;
    t[1].tv_usec = 0;
    success = ::utimes(Utils::StringFromWide(path).c_str(), t) == 0;
    #endif

    return success;
}

