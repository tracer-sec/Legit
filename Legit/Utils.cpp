#include "Utils.hpp"
#include <sstream>
#include <iomanip>
#include <vector>

using namespace Legit;
using namespace std;

wstring Utils::FindAndReplace(wstring src, wstring match, wstring replacement)
{
    auto f = src.find(match);
    while (f != wstring::npos)
    {
        src.replace(f, match.length(), replacement);
        f = src.find(match, f + replacement.length());
    }
    return src;
}

string Utils::Trim(string s)
{
    s = s.substr(s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
    return s;
}

string Utils::Hex(vector<char> data)
{
    stringstream ss;
    ss << hex << setfill('0');
    for (char c : data)
    {
        ss << setw(2) << static_cast<int>(static_cast<unsigned char>(c));
    }
    return ss.str();
}

string Utils::Hex(char *data, size_t length)
{
    vector<char> d(data, data + length);
    return Hex(d);
}

string Utils::FriendlySize(size_t length)
{
    float s = static_cast<float>(length);
    vector<string> prefix = { "", "K", "M", "G", "T" };

    unsigned int i = 0;
    while (s > 1024 && i < prefix.size() - 1)
    {
        i++;
        s /= 1024;
    }

    ostringstream ss;
    ss.precision(2);
    ss << fixed << s << prefix[i] << "B";
    return ss.str();
}

string Utils::Join(string seperator, vector<string> stringList)
{
    ostringstream ss;

    bool first = true;
    for (auto s : stringList)
    {
        if (!first)
        {
            ss << seperator;
        }
        ss << s;
        first = false;
    }

    return ss.str();
}
