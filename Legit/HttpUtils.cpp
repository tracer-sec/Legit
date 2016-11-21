#include "HttpUtils.hpp"
#include "Utils.hpp"
#include "Random.hpp"
#include <sstream>
#include <algorithm>

using namespace Legit;
using namespace std;

// TODO: I really doubt this is comprehensive enough
const unordered_map<string, string> URL_ENCODING = {
    { "\r", "%0D" },
    { "\n", "%0A" },
    { " ", "+" },
    { "\"", "%22" },
    { "#", "%23" },
    { "&", "%26" },
    { "/", "%2F" },
    { "=", "%3D" }
};

string UrlEncode(string s)
{
    for (auto p : URL_ENCODING)
    {
        s = Utils::FindAndReplace(s, p.first, p.second);
    }
    return s;
}

vector<char> HttpUtils::CreateBody(unordered_map<string, string> fields)
{
    ostringstream builder;

    bool join = false;
    for (auto d : fields)
    {
        if (join)
            builder << "&";
        builder << UrlEncode(d.first) << "=" << UrlEncode(d.second);
        join = true;
    }
    string body = builder.str();

    return vector<char>(body.begin(), body.end());
}

vector<char> HttpUtils::CreateBody(unordered_map<string, string> fields, unordered_map<string, HttpFile> files, string &boundary)
{
    // Find boundary that doesn't appear in the request body
    RandomGenerator r;
    bool boundaryInData = true;
    while (boundaryInData)
    {
        boundaryInData = false;
        boundary = "==========================================" + r.GetString(ALPHA_NUMERIC_MIXED, 16);
        for (auto d : fields)
        {
            boundaryInData = boundaryInData 
                || d.first.find(boundary) != string::npos
                || d.second.find(boundary) != string::npos;
        }
        vector<char> boundaryChar(boundary.begin(), boundary.end());
        for (auto f : files)
        {
            boundaryInData = boundaryInData
                || f.first.find(boundary) != string::npos
                || f.second.filename.find(boundary) != string::npos
                || search(f.second.file.begin(), f.second.file.end(), boundaryChar.begin(), boundaryChar.end()) != f.second.file.end();
        }
    }

    vector<char> result;
    string newline = "\r\n";
    for (auto d : fields)
    {
        ostringstream headerBuilder;
        headerBuilder << "--" << boundary << "\r\n";
        headerBuilder << "Content-Disposition: form-data; name=\"" << d.first << "\"\r\n\r\n";
        string sectionHeader = headerBuilder.str();

        result.insert(result.end(), sectionHeader.begin(), sectionHeader.end());
        result.insert(result.end(), d.second.begin(), d.second.end());
        result.insert(result.end(), newline.begin(), newline.end());
    }

    for (auto f : files)
    {
        ostringstream headerBuilder;
        headerBuilder << "--" << boundary << "\r\n";
        headerBuilder << "Content-Disposition: form-data; name=\"" << f.first << "\"; filename=\"" << f.second.filename << "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
        string sectionHeader = headerBuilder.str();

        result.insert(result.end(), sectionHeader.begin(), sectionHeader.end());
        result.insert(result.end(), f.second.file.begin(), f.second.file.end());
        result.insert(result.end(), newline.begin(), newline.end());
    }

    string terminator = "--" + boundary + "--\r\n";
    result.insert(result.end(), terminator.begin(), terminator.end());
        
    return result;
}

vector<string> HttpUtils::SplitUrl(string url)
{
    int offset = 0;
    int found = url.find_first_of("://");
    string scheme;
    if (found != string::npos)
        scheme = url.substr(offset, found + 3);
    offset += scheme.size();
    string host = url.substr(offset, url.find_first_of("/", offset) - offset);
    offset += host.size();
    string path = url.substr(offset);

    // Check to see if a port is specified
    string port = "";
    found = host.find_first_of(':');
    if (found != string::npos)
    {
        port = host.substr(found + 1);
        host = host.substr(0, found);
    }

    return vector<string> { scheme, host, port, path };
}

