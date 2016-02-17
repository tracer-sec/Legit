#include "Utils.h"

using namespace std;

wstring Utils::FindAndReplace(wstring src, wstring match, wstring replacement)
{
    auto f = src.find(match);
    while (f != wstring::npos)
    {
        src.replace(f, f + match.length(), replacement);
        f = src.find(match);
    }
    return src;
}

string Utils::Trim(string s)
{
    s = s.substr(s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
    return s;
}
