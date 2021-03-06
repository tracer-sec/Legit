#include "Socket.hpp"

#ifndef _WIN32
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #define SOCKET_ERROR    (-1)
#endif

#include <sstream>
#include <cstring> // for memset

using namespace Legit;
using namespace std;

bool Socket::initialised_ = false;

int GetSystemError()
{
    #ifdef _WIN32
        return ::WSAGetLastError();
    #else
        return errno;
    #endif
}

Socket::Socket(string host, string service, unsigned short timeout) :
    host_(host)
{
    ostringstream ss;
    ss << host_ << ":" << service;
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
    result = ::getaddrinfo(host_.c_str(), service.c_str(), &hint, &address);
    if (result != 0)
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
        #ifdef _WIN32
        DWORD t = timeout;
        #else
        timeval t;
        t.tv_sec = 0;
        t.tv_usec = timeout * 1000;
        #endif
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
    #ifdef _WIN32
    ::closesocket(socket_);
    #else
    ::close(socket_);
    #endif
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
    auto bytesRead = ::recv(socket_, buffer, length, 0);
    
    if (bytesRead == 0) // Actually closed gracefully, but this makes more sense
        return SOCKET_ERROR;

    if (bytesRead == SOCKET_ERROR)
    {
        // We're assuming it's nonblocking if it chucks any of those. I hate this behaviour
        int err = GetSystemError();
        #ifdef _WIN32
        if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT) // Not just me. Fuck you, Winsock. https://lists.gnupg.org/pipermail/gnutls-devel/2008-June/002384.html
            bytesRead = 0;
        #else
        if (err == EAGAIN || err == EWOULDBLOCK)
            bytesRead = 0;
        #endif
        else
            error_ = "Error during recv";
    }
    return bytesRead;
}

void Socket::Close()
{
    #ifdef _WIN32
    ::closesocket(socket_);
    #else
    ::close(socket_);
    #endif
}

bool Socket::StartUp()
{
    #ifdef _WIN32
    WSAData wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) != SOCKET_ERROR;
    #else
    return true;
    #endif
}

bool Socket::Shutdown()
{
    #ifdef _WIN32
    return WSACleanup() != SOCKET_ERROR;
    #else
    return true;
    #endif
}

