#include "Mocks.hpp"
#include <algorithm>

#undef min

using namespace Legit;
using namespace std;

Mocket::Mocket(string host, string port, ostringstream &sendStream, string response) :
    host_(host),
    sent_(sendStream),
    response_(response)
{

}

int Mocket::Send(const char *buffer, size_t length)
{
    string foo(buffer, length);
    return Send(foo);
}

int Mocket::Send(string s)
{
    sent_ << s;
    return s.length();
}

int Mocket::Receive(char *buffer, size_t length)
{
    auto l = min(length - 1, response_.length());
    memset(buffer, 0, length);

    #ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4996)
    #endif

    response_.copy(buffer, l);

    #ifdef _MSC_VER
    #pragma warning(pop)
    #endif

    return l;
}
