#pragma once

#include <string>
#include <vector>
#include <memory>
#include <botan/tls_client.h>
#include "CertStore.hpp"
#include "Socket.hpp"

namespace Legit
{

class SecureSocket : public Legit::Socket
{
public:
    SecureSocket(std::string host, std::string service, std::unique_ptr<CertStore> credentials, unsigned short timeout = 0);
    virtual ~SecureSocket();

    int Send(const char *buffer, size_t length) override;
    int Send(std::string s) override;
    int Receive(char *buffer, size_t length) override;
    void Close() override;

private:
    void output(const Botan::byte *data, size_t length);
    void data(const Botan::byte *data, size_t length);
    void alert(Botan::TLS::Alert alert, const Botan::byte *data, size_t length);
    bool session(const Botan::TLS::Session &session);

    std::shared_ptr<Botan::TLS::Client> client_;
    std::vector<char> buffer_;
    std::unique_ptr<CertStore> credentials_;
};

} // end namespace
