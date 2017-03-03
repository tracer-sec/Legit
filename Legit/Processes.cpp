#include "Processes.hpp"
#include "Utils.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <TlHelp32.h>
    #include <Psapi.h>
#else
    #include <csignal>
    #include <unistd.h>
    #include <sys/select.h>
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

string Processes::GetExecutablePath()
{
    #ifdef _WIN32

    TCHAR buffer[MAX_PATH + 1];
    DWORD length = ::GetModuleFileName(NULL, buffer, MAX_PATH + 1);
    wstring widePath(&buffer[0], length);
    string path = Utils::StringFromWide(widePath);
    char SEPERATOR = '\\';

    #else

    char pathBuffer[1025] = { 0 };
    readlink("/proc/self/exe", pathBuffer, 1024);
    string path(pathBuffer);
    char SEPERATOR = '/';

    #endif

    auto pos = path.find_last_of(SEPERATOR);
    if (pos == string::npos)
        return path;

    return path.substr(0, pos);
}

string Processes::Command(const string cmd)
{
    string result;

    #ifdef _WIN32

    // TODO

    #else

    const int PIPE_READ = 0;
    const int PIPE_WRITE = 1;

    string fullCommand = cmd + " 2>&1";

    int in[2], out[2];
    if (pipe(in) != 0 || pipe(out) != 0)
        return "* ERROR CREATING PIPES";

    auto pid = fork();
    if (pid == 0)
    {
        // Close out pointless pipes and run command
        close(in[PIPE_WRITE]);
        dup2(in[PIPE_READ], PIPE_READ);
        close(out[PIPE_READ]);
        dup2(out[PIPE_WRITE], PIPE_WRITE);
        execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
        perror("execl");
        exit(1);
    }
    else
    {
        // Start reading contents of stdout pipe
        fd_set fdList;
        FD_ZERO(&fdList);
        FD_SET(out[PIPE_READ], &fdList);

        timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        size_t bytesRead = 1;
        while (bytesRead > 0)
        {
            int ready = select(out[PIPE_READ] + 1, &fdList, nullptr, nullptr, &timeout);
            if (ready < 1)
            {
                // Nothing to read after the timeout. Kill the process and bail
                Processes::KillProcess(pid);
                result += "Command timeout\n";
                break;
            }
            vector<char> buffer(256);
            bytesRead = read(out[PIPE_READ], &buffer[0], buffer.size());
            result += string(&buffer[0], bytesRead);
        }
    }

    #endif

    return result;
}

#ifdef _WIN32

bool Processes::Inject(wstring path, unsigned int processId)
{
    bool success = false;

    auto h = ::OpenProcess(PROCESS_ALL_ACCESS, false, processId);
    if (h != NULL)
    {
        auto kernelLib = ::GetModuleHandle(L"kernel32.dll");
        char *loadLibraryAddress = reinterpret_cast<char *>(::GetProcAddress(kernelLib, "LoadLibraryW"));

        size_t bufferSize = (path.size() + 1) * sizeof(wstring::value_type); // include the null byte
        char *addressSpace = reinterpret_cast<char *>(::VirtualAllocEx(h, NULL, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

        SIZE_T bytesWritten;
        ::WriteProcessMemory(h, addressSpace, path.c_str(), bufferSize, &bytesWritten);

        DWORD newThreadId;
        auto remoteThread = ::CreateRemoteThread(h, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddress), addressSpace, 0, &newThreadId);
        ::WaitForSingleObject(remoteThread, 5000);

        ::VirtualFreeEx(h, addressSpace, path.size(), MEM_RELEASE);
        ::CloseHandle(remoteThread);
        ::CloseHandle(h);

        success = true;
    }

    return success;
}

#endif
