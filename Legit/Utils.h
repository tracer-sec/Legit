#pragma once

#include <string>

namespace Utils
{
    std::wstring FindAndReplace(std::wstring src, std::wstring match, std::wstring replacement);
    std::string Trim(std::string s);
}
