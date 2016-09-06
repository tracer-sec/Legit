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

struct User
{
    std::wstring username;
    std::wstring fullName;
    unsigned int priviledges;
    unsigned int userId;
};

class SystemInfo
{
public:
    SystemInfo();

    std::wstring GetPlatform() { return platform_; }
    std::wstring GetVersion() { return version_; }
    std::vector<Interface> GetInterfaces() { return interfaces_; }
    std::vector<User> GetUsers() { return users_; }

    void Dump(std::wostream &s);

private:
    std::wstring platform_;
    std::wstring version_;
    std::vector<Interface> interfaces_;
    std::wstring currentUser_;
    std::vector<User> users_;
};

} // end namespace
