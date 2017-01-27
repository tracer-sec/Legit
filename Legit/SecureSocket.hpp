#pragma once

#include <string>
#include <vector>
#include <memory>
#include <botan/tls_client.h>
#include "CertStore.hpp"
#include "Socket.hpp"

namespace Legit
{

class SecureSocket : public Legit::Socket, public Botan::TLS::Callbacks
{
public:
    SecureSocket(std::string host, std::string service, std::unique_ptr<CertStore> credentials, unsigned short timeout = 0, bool validate = true);
    virtual ~SecureSocket();

    int Send(const char *buffer, size_t length) override;
    int Send(std::string s) override;
    int Receive(char *buffer, size_t length) override;
    void Close() override;

    void tls_emit_data(const uint8_t data[], size_t size) override;
    void tls_record_received(uint64_t seq_no, const uint8_t data[], size_t size) override;
    void tls_alert(Botan::TLS::Alert alert) override;
    bool tls_session_established(const Botan::TLS::Session &session) override;
    void tls_verify_cert_chain(
        const std::vector<Botan::X509_Certificate> &cert_chain,
        const std::vector<std::shared_ptr<const Botan::OCSP::Response>> &ocsp_responses,
        const std::vector<Botan::Certificate_Store *> &trusted_roots,
        Botan::Usage_Type usage,
        const std::string &hostname, 
        const Botan::TLS::Policy &policy
    ) override;

private:
    std::shared_ptr<Botan::TLS::Client> client_;
    std::vector<char> buffer_;
    std::unique_ptr<CertStore> credentials_;
    bool validate_;
};

} // end namespace
