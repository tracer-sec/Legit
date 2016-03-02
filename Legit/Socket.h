#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

class Socket
{
public:
    Socket(std::string host, std::string service, unsigned short timeout = 0);
    virtual ~Socket();

    virtual int Send(const char *buffer, size_t length);
    virtual int Send(std::string s);
    virtual int Receive(char *buffer, size_t length);
    virtual void Close();

    std::string ToString() { return name_; }
    std::string GetError() { return error_; }

    static bool StartUp();
    static bool Shutdown();

protected:
    std::string error_;

private:
    static bool initialised_;
    SOCKET socket_;
    std::string name_;
};
