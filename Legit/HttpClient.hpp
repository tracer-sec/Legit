#pragma once

#include "Socket.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Legit
{

struct HttpResponse
{
    std::string statusCode;
    std::unordered_map<std::string, std::string> headers;
    std::vector<char> body;
};

class HttpClient
{
public:
    HttpClient(const std::string host);
    HttpClient(std::unique_ptr<ISocket> socket);

    HttpResponse Get(const std::string &url);
    HttpResponse Post(const std::string &url, const std::string &body, const std::string &encoding);
    HttpResponse Put(const std::string &url, const std::string &body, const std::string &encoding);
    HttpResponse Delete(const std::string &url);

    void AddHeader(std::string key, std::string value);
    void RemoveHeader(std::string key);

private:
    HttpResponse SendRequest(std::string request);
    HttpResponse ParseResponse();
    std::vector<char> ReadBytes(size_t length);
    std::vector<char> ReadUntil(const std::string &match);

    std::string host_;
    std::unique_ptr<ISocket> socket_;
    std::vector<char> remains_;
    std::unordered_map<std::string, std::string> headers_;
};

} // end namespace
