#pragma once

#include <string>
#include <vector>

namespace Legit
{

namespace Utils
{
    std::wstring FindAndReplace(std::wstring src, std::wstring match, std::wstring replacement);
    std::string Trim(std::string s);
    std::string Hex(std::vector<char> data);
    std::string Hex(char *data, size_t length);
    std::string FriendlySize(size_t length);
    std::string Join(std::string seperator, std::vector<std::string> stringList);
}

} // end namespace
