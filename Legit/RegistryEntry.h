#pragma once

#include <Windows.h>
#include <string>
#include <vector>

class RegistryEntry
{
public:
    RegistryEntry(HKEY hive, std::wstring subKey);
    ~RegistryEntry();

    DWORD GetDwordValue(std::wstring value);
    std::wstring GetStringValue(std::wstring value);
    void GetBlobValue(std::wstring value, std::vector<unsigned char> &blob);

private:
    HKEY key_;
    long success_;
};