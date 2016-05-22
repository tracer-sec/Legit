#pragma once

#include <string>
#include <vector>
#include <ostream>

namespace Legit
{

struct Interface
{
    std::wstring Mac;
    std::wstring Name;
    std::wstring Address;
};

class SystemInfo
{
public:
    SystemInfo();

    std::wstring GetPlatform() { return platform_; }
    std::wstring GetVersion() { return version_; }
    std::vector<Interface> GetInterfaces() { return interfaces_; }

    void Dump(std::wostream &s);

private:
    std::wstring platform_;
    std::wstring version_;
    std::vector<Interface> interfaces_;
};

} // end namespace
