#include "CertStore.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <wincrypt.h>
#else
    #include "FileSystem.hpp"
    #include "DataLoader.hpp"
#endif
#include <iostream>
#include <botan/data_src.h>

using namespace Legit;
using namespace std;
using namespace Botan;

CertStore::CertStore()
{
    #ifdef _WIN32
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
    #else
    auto files = FileSystem::GetFiles(L"/etc/ssl/certs");
    for (auto f : files)
    {
        vector<char> data;
        DataLoader::LoadFromFile(L"/etc/ssl/certs/" + f, data);
        
        try
        {
            DataSource_Memory certData(reinterpret_cast<Botan::byte *>(&data[0]), data.size());
            X509_Certificate cert(certData);

            //cout << "cert found: " << cert.subject_dn() << " (" << cert.is_CA_cert() << ")" << endl << flush;
            
            certs_.push_back(new Certificate_Store_In_Memory(cert));
        }
        catch (std::exception &e)
        {
            cerr << "cert error: " << e.what() << endl;
        }
    }
    #endif
}

CertStore::CertStore(string pem)
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
