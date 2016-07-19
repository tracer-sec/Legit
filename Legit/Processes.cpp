#include "Processes.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <TlHelp32.h>
    #include <Psapi.h>
#else
    #include<csignal>
#endif

using namespace Legit;
using namespace std;

vector<Process> Processes::GetCurrentProcesses()
{
    vector<Process> result;

    #ifdef _WIN32

    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);

    if (snapshot != INVALID_HANDLE_VALUE)
    {
        if (::Process32First(snapshot, &process) == TRUE)
        {
            while (::Process32Next(snapshot, &process) != FALSE)
            {
                Process p;
                p.processId = process.th32ProcessID;
                p.parentProcessId = process.th32ParentProcessID;
                p.filename = wstring(process.szExeFile);

                HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, p.processId);
                if (processHandle != NULL)
                {
                    TCHAR filename[MAX_PATH];
                    DWORD bufferSize = MAX_PATH;
                    if (::QueryFullProcessImageName(processHandle, 0, filename, &bufferSize) > 0)
                    {
                        p.filename = wstring(filename);
                    }
                    HANDLE tokenHandle;
                    ::OpenProcessToken(processHandle, TOKEN_QUERY, &tokenHandle);
                    if (tokenHandle != 0)
                    {
                        DWORD userDataSize = 0;
                        ::GetTokenInformation(tokenHandle, TokenUser, nullptr, 0, &userDataSize);
                        
                        vector<char> userData(userDataSize);
                        TOKEN_USER *user = reinterpret_cast<TOKEN_USER *>(&userData[0]);

                        DWORD requiredSize = 0;
                        if (::GetTokenInformation(tokenHandle, TokenUser, user, userDataSize, &requiredSize) != 0)
                        {
                            TCHAR name[256];
                            TCHAR domain[256];
                            DWORD nameBufferSize = 256;
                            DWORD domainBufferSize = 256;
                            SID_NAME_USE sidType;

                            if (::LookupAccountSid(NULL, user->User.Sid, name, &nameBufferSize, domain, &domainBufferSize, &sidType) != 0)
                            {
                                p.owner = wstring(name);
                            }
                        }

                        ::CloseHandle(tokenHandle);
                    }

                    ::CloseHandle(processHandle);
                }

                result.push_back(p);
            }

            ::CloseHandle(snapshot);
        }
    }

    #else
    
    // enumerate /proc/ ...

    #endif

    return result;
}

bool Processes::KillProcess(int processId)
{
    bool success = false;

    #ifdef _WIN32

    HANDLE processHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (processHandle != NULL)
    { 
        success = ::TerminateProcess(processHandle, 0) != 0;
        ::CloseHandle(processHandle);
    }

    #else

    success = kill(processId, SIGKILL) == 0;

    #endif

    return success;
}
