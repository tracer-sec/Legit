#include "Socket.hpp"

#include <sstream>

using namespace std;

bool Socket::initialised_ = false;

Socket::Socket(string host, string service, unsigned short timeout)
{
    ostringstream ss;
    ss << host << ":" << service;
    name_ = ss.str();

    if (!initialised_)
    {
        if (!StartUp())
        {
            error_ = "Winsock startup failed";
            return;
        }
        initialised_ = true;
    }

    int result;
    addrinfo *address, hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    result = ::getaddrinfo(host.c_str(), service.c_str(), &hint, &address);
    if (result == SOCKET_ERROR)
    {
        error_ = "Address lookup failed";
        return;
    }

    socket_ = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (socket_ == SOCKET_ERROR)
    {
        error_ = "Socket creation failed";
        return;
    }

    result = ::connect(socket_, address->ai_addr, address->ai_addrlen);
    if (result == SOCKET_ERROR)
    {
        error_ = "Could not connect";
        return;
    }

    ::freeaddrinfo(address);

    if (timeout > 0)
    {
        DWORD t = timeout;
        result = ::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&t), sizeof(t));
        if (result == SOCKET_ERROR)
        {
            error_ = "Could not set timeout";
            return;
        }
    }
}

Socket::~Socket()
{
    ::closesocket(socket_);
}

int Socket::Send(const char *buffer, size_t length)
{
    return ::send(socket_, buffer, length, 0);
}

int Socket::Send(string s)
{
    return Send(s.c_str(), s.length());
}

int Socket::Receive(char *buffer, size_t length)
{
    return ::recv(socket_, buffer, length, 0);
}

void Socket::Close()
{
    ::closesocket(socket_);
}

bool Socket::StartUp()
{
    WSAData wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) != SOCKET_ERROR;
}

bool Socket::Shutdown()
{
    return WSACleanup() != SOCKET_ERROR;
}
