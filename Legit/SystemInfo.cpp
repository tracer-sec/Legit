#include "SystemInfo.hpp"
#include "Utils.hpp"

#ifdef _WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <Lm.h>
#else
#include <sys/utsname.h>
#endif
#include <sstream>

using namespace Legit;
using namespace std;

SystemInfo::SystemInfo()
{
    #ifdef _WIN32
    platform_ = L"Windows";
    
    OSVERSIONINFOEX versionInfo;
    memset(&versionInfo, 0, sizeof(OSVERSIONINFOEX));
    versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    int major = -1;
    int minor = -1;
    int servicePack = -1;

    // This is bullshit. Just saying. Give us back GetVersionEx.
    ULONGLONG mask = ::VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
    for (int i = 5; i < 10; ++i)
    {
        versionInfo.dwMajorVersion = i;
        if (VerifyVersionInfo(&versionInfo, VER_MAJORVERSION, mask))
        {
            major = i;
            break;
        }
    }

    mask = ::VerSetConditionMask(0, VER_MINORVERSION, VER_EQUAL);
    for (int i = 0; i < 10; ++i)
    {
        versionInfo.dwMinorVersion = i;
        if (VerifyVersionInfo(&versionInfo, VER_MINORVERSION, mask))
        {
            minor = i;
            break;
        }
    }

    mask = ::VerSetConditionMask(0, VER_SERVICEPACKMAJOR, VER_EQUAL);
    for (int i = 0; i < 10; ++i)
    {
        versionInfo.wServicePackMajor = i;
        if (VerifyVersionInfo(&versionInfo, VER_SERVICEPACKMAJOR, mask))
        {
            servicePack = i;
            break;
        }
    }

    wostringstream ss;
    ss << major << "." << minor << "." << servicePack;
    version_ = ss.str();
    #else
    utsname versionData;
    ::uname(&versionData);
    string p(versionData.sysname);
    string v(versionData.release);
    platform_ = Utils::WideFromString(p);
    version_ = Utils::WideFromString(v);
    #endif

    #ifdef _WIN32
    // Get adapter MACs
    DWORD size = 0;

    ::GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, nullptr, &size);
    vector<char> buffer(size);
    PIP_ADAPTER_ADDRESSES adapterInfo = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(&buffer[0]);

    if (::GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, adapterInfo, &size) == ERROR_SUCCESS)
    {
        for (; adapterInfo->Next != nullptr; adapterInfo = adapterInfo->Next)
        {
            Interface i;
            string hex = Utils::Hex(reinterpret_cast<char *>(adapterInfo->PhysicalAddress), static_cast<size_t>(adapterInfo->PhysicalAddressLength));
            i.Mac = Utils::WideFromString(hex);
            i.Name = wstring(adapterInfo->FriendlyName);

            if (adapterInfo->FirstUnicastAddress != nullptr)
            {
                auto unicastAddress = adapterInfo->FirstUnicastAddress->Address;
                DWORD bufferSize = 100;
                vector<wchar_t> ipBuffer(100);
                auto foo = ::WSAAddressToString(unicastAddress.lpSockaddr, unicastAddress.iSockaddrLength, NULL, reinterpret_cast<LPTSTR>(&ipBuffer[0]), &bufferSize);
                i.Address = wstring(ipBuffer.begin(), ipBuffer.begin() + bufferSize - 1);
            }

            interfaces_.push_back(i);
        }
    }
    #endif

    #ifdef _WIN32
    vector<wchar_t> usernameBuffer(UNLEN + 1);
    size = UNLEN + 1;
    if (::GetUserName(&usernameBuffer[0], &size) != 0)
    {
        currentUser_ = wstring(usernameBuffer.begin(), usernameBuffer.begin() + size);
    }
    #else
    vector<char> usernameBuffer(256);
    if (getlogin_r(&usernameBuffer[0], 256) == 0)
    {
        string s(usernameBuffer);
        currentUser_ = Utils::WideFromString(s);
    }
    #endif

    #ifdef _WIN32
    USER_INFO_3 *userInfo;
    DWORD entriesRead;
    DWORD totalEntries;
    DWORD resumeHandle = 0;

    DWORD result = ::NetUserEnum(nullptr, 3, 0, reinterpret_cast<LPBYTE *>(&userInfo), MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, &resumeHandle);
    if (result == NERR_Success)
    {
        for (unsigned int i = 0; i < entriesRead; ++i)
        {
            User u;
            u.username = wstring(userInfo[i].usri3_name);
            u.fullName = wstring(userInfo[i].usri3_full_name);
            u.priviledges = userInfo[i].usri3_priv;
            u.userId = userInfo[i].usri3_user_id;
            users_.push_back(u);
        }
    }
    ::NetApiBufferFree(userInfo);
    #endif

}

void SystemInfo::Dump(wostream &s)
{
    s << L"Platform: " << platform_ << endl;
    s << L"Version: " << version_ << endl;
    for (auto i : interfaces_)
    {
        s << i.Mac << "  ";
        s << i.Name << "  ";
        s << i.Address << endl;
    }
}
