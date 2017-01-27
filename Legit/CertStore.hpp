#pragma once

#include <botan/credentials_manager.h>
#include <vector>

namespace Legit
{

class CertStore : public Botan::Credentials_Manager
{
public:
    CertStore();
    CertStore(std::string pem);

    std::vector<Botan::Certificate_Store *> trusted_certificate_authorities(
        const std::string &type, 
        const std::string &context) override;

    std::vector<Botan::X509_Certificate> cert_chain(
        const std::vector<std::string> &algos,
        const std::string &type,
        const std::string &hostname) override;

    Botan::Private_Key* private_key_for(const Botan::X509_Certificate& cert,
        const std::string &type,
        const std::string &context) override;

private:
    std::vector<Botan::Certificate_Store *> certs_;
};

} // end namespace
