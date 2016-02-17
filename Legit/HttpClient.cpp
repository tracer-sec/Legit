#include "HttpClient.h"
#include "Utils.h"

#include <Ws2tcpip.h>
#include <sstream>

using namespace std;

const string USER_AGENT = "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:32.0) Gecko/20100101 Firefox/32.0";

HttpClient::HttpClient(const string host) : 
    host_(host),
    socket_(host, "80")
{

}

HttpResponse HttpClient::ParseResponse()
{
    HttpResponse response;
    // Read response status
    response.statusCode = ReadUntil("\r\n");
    // Read response header
    string header = ReadUntil("\r\n");
    while (header.length() > 0)
    {
        auto offset = header.find(':');
        response.headers.insert(make_pair(header.substr(0, offset), Utils::Trim(header.substr(offset + 1))));
        header = ReadUntil("\r\n");
    }
    // Read response body
    if (response.headers["Transfer-Encoding"] == "chunked")
    {
        ostringstream ss;
        string line = ReadUntil("\r\n");
        int chunkSize = stoi(line.c_str(), 0, 16);
        while (chunkSize > 0)
        {
            string chunk = ReadBytes(chunkSize);
            ss << chunk;
            string line = ReadUntil("\r\n"); // Skip the blank line between chunks
            line = ReadUntil("\r\n");
            chunkSize = stoi(line.c_str(), 0, 16);
        }
        response.body = ss.str();
    }
    else
    {
        int contentLength = atoi(response.headers["Content-Length"].c_str());
        response.body = ReadBytes(contentLength);
    }

    return response;
}

string HttpClient::ReadBytes(size_t length)
{
    char buffer[4096];
    ostringstream ss;
    ss << remains_;
    size_t totalBytes = remains_.length();

    while (totalBytes < length)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = socket_.Receive(buffer, min(sizeof(buffer), length - totalBytes));
        ss << string(buffer, 0, bytesRead);
        totalBytes += bytesRead;
    }

    remains_ = "";
    return ss.str();
}

string HttpClient::ReadUntil(const string &match)
{
    // Is it sitting in the leftover buffer?
    auto gotit = remains_.find(match);
    if (gotit != string::npos)
    {
        string result = remains_.substr(0, gotit);
        remains_ = remains_.substr(gotit + match.length());
        return result;
    }

    // Guess not, start consuming the stream again
    char buffer[4097];
    ostringstream ss;
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = socket_.Receive(buffer, sizeof(buffer) - 1);
    char *find = strstr(buffer, match.c_str());
    ss << remains_;
    while (find == nullptr && bytesRead > 0)
    {
        ss << buffer;
        memset(buffer, 0, sizeof(buffer));
        bytesRead = socket_.Receive(buffer, sizeof(buffer) - 1);
    }
    string s = string(buffer, find - buffer);
    ss << s;
    remains_ = find + match.length();
    return ss.str();
}

HttpResponse HttpClient::SendRequest(string request)
{
    int bytesSent = socket_.Send(request.c_str(), request.length());

    HttpResponse response = ParseResponse();

    return response;
}

HttpResponse HttpClient::Get(const string &url)
{
    ostringstream ss;
    ss << "GET " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    ss << "User-Agent: " << USER_AGENT << "\r\n";
    ss << "\r\n";

    return SendRequest(ss.str());
}

HttpResponse HttpClient::Post(const string &url, const string &body, const string &encoding)
{
    ostringstream ss;
    ss << "POST " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    ss << "User-Agent: " << USER_AGENT << "\r\n";
    ss << "Content-Length: " << body.length() << "\r\n";
    ss << "Content-Type: " << encoding << "\r\n";
    ss << "\r\n";

    ss << body;

    return SendRequest(ss.str());
}

HttpResponse HttpClient::Put(const string &url, const string &body, const string &encoding)
{
    ostringstream ss;
    ss << "PUT " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    ss << "User-Agent: " << USER_AGENT << "\r\n";
    ss << "Content-Length: " << body.length() << "\r\n";
    ss << "Content-Type: " << encoding << "\r\n";
    ss << "\r\n";

    ss << body;

    return SendRequest(ss.str());
}

HttpResponse HttpClient::Delete(const string &url)
{
    ostringstream ss;
    ss << "DELETE " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    ss << "User-Agent: " << USER_AGENT << "\r\n";
    ss << "\r\n";

    return SendRequest(ss.str());
}
