#pragma once

#include <string>
#include <vector>

namespace Legit
{

struct Process
{
    int processId;
    int parentProcessId;
    std::wstring filename;
    std::wstring owner;
};

namespace Processes
{
    std::vector<Process> GetCurrentProcesses();
    bool KillProcess(int processId);
}

} // end namespace
