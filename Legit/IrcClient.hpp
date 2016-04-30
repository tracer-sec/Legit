#pragma once

#include "Socket.hpp"

#include <string>
#include <vector>
#include <memory>

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
    IrcClient(std::unique_ptr<Socket> socket, std::string nick);
    IrcClient(std::string host, std::string port, std::string nick);
    
    void Send(std::string message);
    IrcMessage Receive();

    std::string GetError() { return error_; }
    bool ReceivedFirstPing() { return receivedFirstPing_; }

private:
    IrcMessage ParseMessage(std::string m);

    std::string error_;
    std::unique_ptr<Socket> socket_;
    std::string remains_;
    bool receivedFirstPing_;
};

} // end namespace
