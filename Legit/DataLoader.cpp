#include "DataLoader.hpp"
#include "Utils.hpp"
#include <fstream>
#ifdef _WIN32
    #include <Windows.h>
#endif

using namespace Legit;
using namespace std;

#ifdef _WIN32

bool DataLoader::LoadFromResource(wstring name, vector<char> &data)
{
    bool result = false;
    
    auto resourceHandle = ::FindResource(nullptr, name.c_str(), RT_RCDATA);
    if (resourceHandle != nullptr)
    {
        auto dataHandle = ::LoadResource(nullptr, resourceHandle);
        if (dataHandle != nullptr)
        {
            size_t size = ::SizeofResource(nullptr, resourceHandle);
            auto resourceData = reinterpret_cast<char *>(::LockResource(dataHandle));
            if (resourceData != nullptr)
            {
                data = vector<char>(resourceData, resourceData + size);
                result = true;
            }
        }
    }
    return result;
}

#endif

bool DataLoader::LoadFromFile(wstring path, vector<char> &data)
{
    bool result = false;

    ifstream f(Utils::StringFromWide(path), ios::binary | ios::ate);
    if (f)
    {
        streamoff length = f.tellg();
        f.seekg(0, f.beg);

        data.resize(static_cast<size_t>(length));
        f.read(&data[0], length);

        f.close();
        result = true;
    }

    return result;
}

bool DataLoader::DumpToFile(wstring path, vector<char> &data)
{
    bool result = false;

    ofstream f(Utils::StringFromWide(path), ios::binary | ios::ate);
    if (f)
    {
        f.write(&data[0], data.size());
        f.close();
        result = true;
    }

    return result;
}
