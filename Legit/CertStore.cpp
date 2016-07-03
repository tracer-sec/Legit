#include "CertStore.hpp"

#include <Windows.h>
#include <wincrypt.h>
#include <iostream>

using namespace Legit;
using namespace std;
using namespace Botan;

CertStore::CertStore() :
    validate_(true)
{
    HANDLE certStore = ::CertOpenSystemStore(NULL, L"ROOT");
    PCCERT_CONTEXT certContext = ::CertEnumCertificatesInStore(certStore, nullptr);

    while (certContext != nullptr)
    {
        try
        {
            DataSource_Memory certData(reinterpret_cast<Botan::byte *>(certContext->pbCertEncoded), certContext->cbCertEncoded);
            X509_Certificate cert(certData);

            //cout << "cert found: " << cert.subject_dn() << " (" << cert.is_CA_cert() << ")" << endl << flush;
            
            certs_.push_back(new Certificate_Store_In_Memory(cert));
        }
        catch (std::exception &e)
        {
            cerr << "cert error: " << e.what() << endl;
        }

        // This also frees the previous cert
        certContext = ::CertEnumCertificatesInStore(certStore, certContext);
    }

    ::CertCloseStore(certStore, 0);
}

CertStore::CertStore(string pem) :
    validate_(true)
{
    DataSource_Memory certData(pem);
    X509_Certificate cert(certData);
    certs_.push_back(new Certificate_Store_In_Memory(cert));
}

CertStore::CertStore(string pem, bool validate) :
    validate_(validate)
{
    DataSource_Memory certData(pem);
    X509_Certificate cert(certData);
    certs_.push_back(new Certificate_Store_In_Memory(cert));
}

vector<Certificate_Store *> CertStore::trusted_certificate_authorities(
    const string &type, 
    const string &context
)
{
    cout << "Getting trusted certs for " << type << "|" << context << endl << flush;

    return certs_;
}

vector<X509_Certificate> CertStore::cert_chain(
    const vector<string>& algos,
    const string& type,
    const string& hostname)
{
    cout << "Getting cert chain for " << type << "|" << hostname << endl << flush;

    return vector<X509_Certificate>();
}

Botan::Private_Key *CertStore::private_key_for(
    const Botan::X509_Certificate& cert,
    const std::string& type,
    const std::string& context)
{
    cout << "Getting private key for " << type << "|" << context << endl << flush;

    return nullptr;
}

// TODO: better handling
void CertStore::verify_certificate_chain(
    const std::string& type,
    const std::string& purported_hostname,
    const std::vector<Botan::X509_Certificate>& cert_chain)
{
    cout << "verify_certificate_chain " << type << "|" << purported_hostname << endl << flush;

    try
    {
        if (validate_)
        {
            Credentials_Manager::verify_certificate_chain(type, purported_hostname, cert_chain);
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        throw;
    }
}