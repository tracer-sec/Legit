#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <vector>

namespace Legit
{

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

} // end namespace

#endif

