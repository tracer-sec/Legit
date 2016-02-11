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
