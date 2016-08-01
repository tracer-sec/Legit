#pragma once

#include <string>
#include <vector>

namespace Legit
{

namespace Utils
{
    std::string FindAndReplace(std::string src, const std::string &match, const std::string &replacement);
    std::wstring FindAndReplace(std::wstring src, const std::wstring &match, const std::wstring &replacement);
    std::string Trim(std::string s);
    std::string Hex(std::vector<char> data);
    std::string Hex(char *data, size_t length);
    std::string FriendlySize(size_t length);
    std::string Join(std::string seperator, std::vector<std::string> stringList);
    std::vector<std::string> Split(std::string s, std::string seperator);
    std::wstring WideFromString(const std::string &s);
    std::string StringFromWide(const std::wstring &w);
}

} // end namespace
