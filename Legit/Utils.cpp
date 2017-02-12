#include "Utils.hpp"
#include <sstream>
#include <iomanip>
#include <vector>

#ifdef _WIN32
#include <codecvt>
#endif

using namespace Legit;
using namespace std;

template<class T>
basic_string<T> FindAndReplaceWorker(basic_string<T> &src, const basic_string<T> &match, const basic_string<T> &replacement)
{
    auto f = src.find(match);
    while (f != string::npos)
    {
        src.replace(f, match.length(), replacement);
        f = src.find(match, f + replacement.length());
    }
    return src;
}

string Utils::FindAndReplace(string src, const string &match, const string &replacement)
{
    return FindAndReplaceWorker<string::value_type>(src, match, replacement);
}

wstring Utils::FindAndReplace(wstring src, const wstring &match, const wstring &replacement)
{
    return FindAndReplaceWorker<wstring::value_type>(src, match, replacement);
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

template<class T>
vector<basic_string<T>> SplitWorker(basic_string<T> &s, basic_string<T> &seperator)
{
    vector<basic_string<T>> result;

    size_t start = 0;
    size_t end = s.find(seperator, 0);

    while (end != string::npos)
    {
        result.push_back(s.substr(start, end - start));
        start = end + seperator.length();
        end = s.find(seperator, start);
    }
    result.push_back(s.substr(start));

    return result;
}

vector<string> Utils::Split(string s, string seperator)
{
    return SplitWorker<string::value_type>(s, seperator);
}

vector<wstring> Utils::Split(wstring s, wstring seperator)
{
    return SplitWorker<wstring::value_type>(s, seperator);
}

wstring Utils::WideFromString(const string &s)
{
    #ifdef _WIN32
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(s);
    #else
    return wstring(s.begin(), s.end());
    #endif
}

string Utils::StringFromWide(const wstring &w)
{
    #ifdef _WIN32
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(w);
    #else
    return string(w.begin(), w.end());
    #endif
}

vector<wstring> Utils::Tokenise(const wstring &s)
{
    vector<wstring> result;

    auto split = Utils::Split(s, L" ");
    wstring current;
    bool insideQuote = false;
    for (auto t : split)
    {
        if (!insideQuote && t[0] == '\"')
        {
            insideQuote = true;
            t = t.substr(1);
        }

        if (insideQuote && t[t.length() - 1] == '\"')
        {
            if (t[t.length() - 2] != '\\')
            {
                insideQuote = false;
                t = t.substr(0, t.length() - 1);
                t = Utils::FindAndReplace(t, L"\\\"", L"\"");
            }
        }

        if (insideQuote)
        {
            current += Utils::FindAndReplace(t, L"\\\"", L"\"") + L" ";
            continue;
        }
        
        current += t;

        result.push_back(current);
        current = L"";
    }

    return result;
}

