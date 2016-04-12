#include "RegistryEntry.hpp"

using namespace std;

RegistryEntry::RegistryEntry(HKEY hive, wstring subKey)
{
    success_ = ::RegOpenKeyEx(hive, subKey.c_str(), 0, KEY_READ, &key_);
}

RegistryEntry::~RegistryEntry()
{
    ::RegCloseKey(key_);
}

DWORD RegistryEntry::GetDwordValue(wstring value)
{
    DWORD result = 0;
    if (success_ == ERROR_SUCCESS)
    {
        DWORD bufferSize = sizeof(DWORD);
        auto success = ::RegQueryValueEx(key_, value.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&result), &bufferSize);
    }
    return result;
}

wstring RegistryEntry::GetStringValue(wstring value)
{
    wstring result;
    if (success_ == ERROR_SUCCESS)
    {
        vector<wchar_t> buffer;
        DWORD bufferSize = buffer.size();
        auto success = ::RegQueryValueEx(key_, value.c_str(), NULL, NULL, NULL, &bufferSize);
        if (success == ERROR_SUCCESS)
        {
            buffer.resize(bufferSize / sizeof(wchar_t));
            auto success = ::RegQueryValueEx(key_, value.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&buffer[0]), &bufferSize);
            if (success == ERROR_SUCCESS)
                result = wstring(buffer.begin(), buffer.end() - 1); // Strip null terminator
        }
    }
    return result;
}

void RegistryEntry::GetBlobValue(wstring value, vector<unsigned char> &buffer)
{
    if (success_ == ERROR_SUCCESS)
    {
        DWORD bufferSize = buffer.size();
        auto success = ::RegQueryValueEx(key_, value.c_str(), NULL, NULL, NULL, &bufferSize);
        if (success == ERROR_SUCCESS)
        {
            buffer.resize(bufferSize);
            success = ::RegQueryValueEx(key_, value.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&buffer[0]), &bufferSize);
        }
    }
}
