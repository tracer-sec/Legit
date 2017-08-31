#include "SecureSocket.hpp"
#include "Environment.hpp"
#include <botan/auto_rng.h>

// TODO: remove
#include <iostream>

#ifndef _WIN32
    #define SOCKET_ERROR    (-1)
#endif

using namespace Legit;
using namespace std;
using namespace std::placeholders;

class RevocationNotRequiredPolicy : public Botan::TLS::Policy
{
public:
    bool require_cert_revocation_info() const override { return false; }
};

SecureSocket::SecureSocket(string host, string service, unique_ptr<CertStore> credentials, unsigned short timeout, bool validate) :
    Socket(host, service, timeout),
    client_(nullptr),
    credentials_(move(credentials)),
    validate_(validate)
{
    if (credentials_ == nullptr)
    {
        credentials_ = make_unique<CertStore>();
    }

    Botan::AutoSeeded_RNG rng;
    Botan::TLS::Session_Manager_In_Memory sessionManager(rng);
    RevocationNotRequiredPolicy policy;
    Botan::TLS::Server_Information server(host, service);

    auto version = policy.latest_supported_version(false);

    client_ = make_shared<Botan::TLS::Client>(
        *this,
        sessionManager, *credentials_, policy, rng,
        server, version);

    cout << "application_protocol: " << client_->application_protocol() << endl << flush;
    cout << "active/closed: " << client_->is_active() << "/" << client_->is_closed() << endl << flush;

    auto foo = client_->application_protocol();

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

void SecureSocket::tls_emit_data(const uint8_t data[], size_t size)
{
    size_t offset = 0;
    while (offset < size)
    {
        size_t sent = Socket::Send(reinterpret_cast<const char *>(data + offset), size - offset);
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

void SecureSocket::tls_record_received(uint64_t seq_no, const uint8_t data[], size_t size)
{
    buffer_.insert(buffer_.end(), data, data + size);
}

void SecureSocket::tls_alert(Botan::TLS::Alert alert)
{
    cout << alert.type_string() << endl << flush;
}

bool SecureSocket::tls_session_established(const Botan::TLS::Session &session)
{
    cout << session.server_info().hostname() << endl << flush;
    return true;
}

// TODO: better handling
void SecureSocket::tls_verify_cert_chain(
    const vector<Botan::X509_Certificate> &cert_chain,
    const vector<shared_ptr<const Botan::OCSP::Response>> &ocsp_responses,
    const vector<Botan::Certificate_Store *> &trusted_roots,
    Botan::Usage_Type usage,
    const string &hostname,
    const Botan::TLS::Policy &policy
)
{
    cout << "verify_certificate_chain " /* << type << "|" */ << hostname << endl << flush;

    try
    {
        if (validate_)
        {
            Botan::TLS::Callbacks::tls_verify_cert_chain(cert_chain, ocsp_responses, trusted_roots, usage, hostname, policy);
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        throw;
    }
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

        if (totalRead > 0)
            break;
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

