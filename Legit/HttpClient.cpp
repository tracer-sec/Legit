#include "HttpClient.hpp"
#include "Utils.hpp"
#include "Environment.hpp"

#include <sstream>
#include <algorithm>
#include <cstring> // for memset

using namespace Legit;
using namespace std;

HttpClient::HttpClient(const string host) : 
    host_(host),
    socket_(make_unique<Socket>(host, "80"))
{
    headers_["User-Agent"] = "Legit v0";
}

HttpClient::HttpClient(unique_ptr<ISocket> s) :
    socket_(move(s))
{
    if (socket_ != nullptr && socket_->GetError() == "")
        host_ = socket_->GetHost();
    headers_["User-Agent"] = "Legit v0";
}

HttpResponse HttpClient::ParseResponse()
{
    HttpResponse response;
    // Read response status
    auto statusLine = ReadUntil("\r\n");
    response.statusCode = string(statusLine.begin(), statusLine.end());
    // Read response header
    vector<char> header = ReadUntil("\r\n");
    while (header.size() > 0)
    {
        auto offset = find(header.begin(), header.end(), ':');
        response.headers.insert(make_pair(string(header.begin(), offset), Utils::Trim(string(offset + 1, header.end()))));
        header = ReadUntil("\r\n");
    }
    // Read response body
    if (response.headers.find("Transfer-Encoding") != response.headers.end() 
        && response.headers["Transfer-Encoding"] == "chunked")
    {
        ostringstream ss;
        vector<char> line = ReadUntil("\r\n");
        int chunkSize = stoi(string(line.begin(), line.end()), 0, 16);
        while (chunkSize > 0)
        {
            vector<char> chunk = ReadBytes(chunkSize);
            response.body.insert(response.body.end(), chunk.begin(), chunk.end());
            line = ReadUntil("\r\n"); // Skip the newline between chunks
            line = ReadUntil("\r\n");
            chunkSize = stoi(string(line.begin(), line.end()), 0, 16);
        }
    }
    else
    {
        int contentLength = atoi(response.headers["Content-Length"].c_str());
        response.body = ReadBytes(contentLength);
    }

    return response;
}

vector<char> HttpClient::ReadBytes(size_t length)
{
    char buffer[4096];
    vector<char> result;
    result.insert(result.end(), remains_.begin(), remains_.begin() + min(length, remains_.size()));
    remains_.erase(remains_.begin(), remains_.begin() + result.size());

    while (result.size() < length)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = socket_->Receive(buffer, min(sizeof(buffer), length - result.size()));
        result.insert(result.end(), buffer, buffer + bytesRead);
    }

    return result;
}

vector<char> HttpClient::ReadUntil(const string &match)
{
    vector<char> result;

    // Is it sitting in the leftover buffer?
    auto gotit = search(remains_.begin(), remains_.end(), match.begin(), match.end());
    if (gotit != remains_.end())
    {
        result.insert(result.end(), remains_.begin(), gotit);
        remains_ = vector<char>(gotit + match.length(), remains_.end());
        return result;
    }

    // Guess not, start consuming the stream again
    char buffer[4097];
    result.insert(result.end(), remains_.begin(), remains_.end());
    
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = socket_->Receive(buffer, sizeof(buffer) - 1);
    char *find = strstr(buffer, match.c_str());
    while (find == nullptr && bytesRead > 0)
    {
        result.insert(result.end(), buffer, buffer + bytesRead);
        memset(buffer, 0, sizeof(buffer));
        bytesRead = socket_->Receive(buffer, sizeof(buffer) - 1);
        find = strstr(buffer, match.c_str());
    }
    if (find != nullptr)
    {
        result.insert(result.end(), buffer, buffer + (find - buffer));
        remains_ = vector<char>(find + match.length(), buffer + bytesRead);
    }
    return result;
}

HttpResponse HttpClient::SendRequest(string request)
{
    int bytesSent = socket_->Send(request.c_str(), request.length());

    HttpResponse response = ParseResponse();

    return response;
}

HttpResponse HttpClient::Get(const string &url)
{
    if (socket_ == nullptr || socket_->GetError() != "")
        return HttpResponse();

    ostringstream ss;
    ss << "GET " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    for (auto h : headers_)
        ss << h.first << ": " << h.second << "\r\n";
    ss << "\r\n";

    return SendRequest(ss.str());
}

HttpResponse HttpClient::Post(const string &url, const vector<char> &body, const string &encoding)
{
    if (socket_ == nullptr || socket_->GetError() != "")
        return HttpResponse();

    string bodyStr(body.begin(), body.end());
    return Post(url, bodyStr, encoding);
}

HttpResponse HttpClient::Post(const string &url, const string &body, const string &encoding)
{
    if (socket_ == nullptr || socket_->GetError() != "")
        return HttpResponse();

    ostringstream ss;
    ss << "POST " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    for (auto h : headers_)
        ss << h.first << ": " << h.second << "\r\n";
    ss << "Content-Length: " << body.length() << "\r\n";
    ss << "Content-Type: " << encoding << "\r\n";
    ss << "\r\n";

    ss << body;

    return SendRequest(ss.str());
}

HttpResponse HttpClient::Put(const string &url, const string &body, const string &encoding)
{
    if (socket_ == nullptr || socket_->GetError() != "")
        return HttpResponse();

    ostringstream ss;
    ss << "PUT " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    for (auto h : headers_)
        ss << h.first << ": " << h.second << "\r\n";
    ss << "Content-Length: " << body.length() << "\r\n";
    ss << "Content-Type: " << encoding << "\r\n";
    ss << "\r\n";

    ss << body;

    return SendRequest(ss.str());
}

HttpResponse HttpClient::Delete(const string &url)
{
    if (socket_ == nullptr || socket_->GetError() != "")
        return HttpResponse();

    ostringstream ss;
    ss << "DELETE " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    for (auto h : headers_)
        ss << h.first << ": " << h.second << "\r\n";
    ss << "\r\n";

    return SendRequest(ss.str());
}

void HttpClient::AddHeader(string key, string value)
{
    headers_[key] = value;
}

void HttpClient::RemoveHeader(string key)
{
    headers_.erase(key);
}

