#pragma once

#include "Socket.hpp"

#include <string>
#include <unordered_map>

namespace Legit
{

struct HttpResponse
{
    std::string statusCode;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HttpClient
{
public:
    HttpClient(const std::string host);

    HttpResponse Get(const std::string &url);
    HttpResponse Post(const std::string &url, const std::string &body, const std::string &encoding);
    HttpResponse Put(const std::string &url, const std::string &body, const std::string &encoding);
    HttpResponse Delete(const std::string &url);

    void AddHeader(std::string key, std::string value);
    void RemoveHeader(std::string key);

private:
    HttpResponse SendRequest(std::string request);
    HttpResponse ParseResponse();
    std::string ReadBytes(size_t length);
    std::string ReadUntil(const std::string &match);

    std::string host_;
    sockaddr_in address_;
    Socket socket_;
    std::string remains_;
    std::unordered_map<std::string, std::string> headers_;
};

} // end namespace