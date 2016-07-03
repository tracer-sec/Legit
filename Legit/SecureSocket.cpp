#include "SecureSocket.hpp"
#include <botan/auto_rng.h>

// TODO: remove
#include <iostream>

#ifndef _WIN32
    #include "Shims.hpp"
    #define SOCKET_ERROR    (-1)
#endif

using namespace Legit;
using namespace std;
using namespace std::placeholders;

SecureSocket::SecureSocket(string host, string service, unique_ptr<CertStore> credentials, unsigned short timeout) :
    Socket(host, service, timeout),
    credentials_(move(credentials)),
    client_(nullptr)
{
    if (credentials_ == nullptr)
    {
        credentials_ = make_unique<CertStore>();
    }

    Botan::AutoSeeded_RNG rng;
    Botan::TLS::Session_Manager_In_Memory sessionManager(rng);
    Botan::TLS::Policy policy;
    Botan::TLS::Server_Information server(host, service);

    auto version = policy.latest_supported_version(false);

    auto outputFunc = std::bind(&SecureSocket::output, this, _1, _2);
    auto dataFunc = std::bind(&SecureSocket::data, this, _1, _2);
    auto alertFunc = std::bind(&SecureSocket::alert, this, _1, _2, _3);
    auto sessionFunc = std::bind(&SecureSocket::session, this, _1);

    client_ = make_shared<Botan::TLS::Client>(
        outputFunc, dataFunc, alertFunc, sessionFunc,
        sessionManager, *credentials_, policy, rng,
        server, version);

    cout << "application_protocol: " << client_->application_protocol() << endl << flush;
    cout << "active/closed: " << client_->is_active() << "/" << client_->is_closed() << endl << flush;

    while (!client_->is_active() && !client_->is_closed())
    {
        char buffer[1024] = { 0 };
        int bytesRead = Socket::Receive(buffer, 1024);

        if (bytesRead == SOCKET_ERROR)
        {
            #ifdef _WIN32
            auto foo = ::WSAGetLastError();
            #endif
            error_ = "Error performing TLS handshake";
            break;
        }

        client_->received_data((Botan::byte *)buffer, bytesRead);
    }

    //auto foo = client_->peer_cert_chain();
    //if (foo.size() > 0)
    //{
    //    auto pem = foo[0].PEM_encode();
    //    cout << pem;
    //}

    cout << "application_protocol: " << client_->application_protocol() << endl << flush;
    cout << "active/closed: " << client_->is_active() << "/" << client_->is_closed() << endl << flush;
}

SecureSocket::~SecureSocket()
{
    client_->close();
}

void SecureSocket::output(const Botan::byte *data, size_t length)
{
    size_t offset = 0;
    while (offset < length)
    {
        size_t sent = Socket::Send(reinterpret_cast<const char *>(data + offset), length - offset);
        if (sent == SOCKET_ERROR)
        {
            #ifdef _WIN32
            auto foo = ::WSAGetLastError();
            #endif
            break;
        }
        offset += sent;
    }
}

void SecureSocket::data(const Botan::byte *data, size_t length)
{
    buffer_.insert(buffer_.end(), data, data + length);
}

void SecureSocket::alert(Botan::TLS::Alert alert, const Botan::byte *data, size_t length)
{
    cout << alert.type_string() << endl << flush;
}

bool SecureSocket::session(const Botan::TLS::Session &session)
{
    cout << session.server_info().hostname() << endl << flush;
    return true;
}

int SecureSocket::Send(const char *buffer, size_t length)
{
    client_->send(reinterpret_cast<const Botan::byte *>(buffer), length);

    return length; // Probably!
}

int SecureSocket::Send(string s)
{
    return Send(s.c_str(), s.length());
}

int SecureSocket::Receive(char *buffer, size_t length)
{
    char socketBuffer[1024] = { 0 };

    size_t totalRead = 0;
    while (totalRead < length)
    {
        if (buffer_.size() > 0) // Got some bytes left in the buffer, grab those
        {
            size_t byteCount = length - totalRead;
            if (byteCount > buffer_.size())
                byteCount = buffer_.size();

            memcpy(buffer + totalRead, &buffer_[0], byteCount);
            //copy_n(buffer_.begin(), byteCount, buffer + totalRead);
            buffer_.erase(buffer_.begin(), buffer_.begin() + byteCount);
            totalRead += byteCount;
        }
        else
        {
            size_t bytesReadFromSocket = Socket::Receive(socketBuffer, 1024);
            if (bytesReadFromSocket == 0 || bytesReadFromSocket == SOCKET_ERROR)
            {
                #ifdef _WIN32
                auto foo = ::WSAGetLastError();
                #endif
                break;
            }
            client_->received_data((Botan::byte *)socketBuffer, bytesReadFromSocket);
        }
    }

    return totalRead;
}

void SecureSocket::Close()
{
    client_->close();
    Socket::Close();
}

