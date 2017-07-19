#pragma once

#include <string>
#include <vector>

namespace Legit
{

struct Process
{
    unsigned int processId;
    unsigned int parentProcessId;
    std::wstring filename;
    std::wstring owner;
};

namespace Processes
{
    std::vector<Process> GetCurrentProcesses();
    bool KillProcess(int processId);
    std::string GetExecutablePath();
    std::string Command(const std::string cmd);
    void Execute(const std::string path);
    
    #ifdef _WIN32
    bool Inject(std::wstring path, unsigned int processId);
    #endif
}

} // end namespace
