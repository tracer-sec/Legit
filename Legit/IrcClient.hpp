#pragma once

#include "Socket.hpp"

#include <string>
#include <vector>

namespace Legit
{

struct IrcMessage
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;

    std::string GetReturnName();
};

class IrcClient
{
public:
    IrcClient(std::string host, std::string port, std::string nick);
    
    void Send(std::string message);
    IrcMessage Receive();

    std::string GetError() { return error_; }

private:
    IrcMessage ParseMessage(std::string m);

    std::string error_;
    Socket socket_;
    std::string remains_;
};

} // end namespace
