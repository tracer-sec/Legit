#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

namespace Legit
{
class ISocket
{
public:
    virtual ~ISocket() { }
    virtual int Send(const char *buffer, size_t length) = 0;
    virtual int Send(std::string s) = 0;
    virtual int Receive(char *buffer, size_t length) = 0;
    virtual void Close() = 0;

    virtual std::string ToString() = 0;
    virtual std::string GetError() = 0;
    virtual std::string GetHost() = 0;
};

class Socket : public ISocket
{
public:
    Socket(std::string host, std::string service, unsigned short timeout = 0);
    virtual ~Socket();

    virtual int Send(const char *buffer, size_t length) override;
    virtual int Send(std::string s) override;
    virtual int Receive(char *buffer, size_t length) override;
    virtual void Close() override;

    virtual std::string ToString() override { return name_; }
    virtual std::string GetError() override { return error_; }
    virtual std::string GetHost() override { return host_; }

    static bool StartUp();
    static bool Shutdown();

protected:
    std::string error_;

private:
    static bool initialised_;
    SOCKET socket_;
    std::string name_;
    std::string host_;
};

} // end namespace
