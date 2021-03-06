#include "IrcClient.hpp"
#include "Environment.hpp"

#include <sstream>
#include <cstring> // for memset

using namespace Legit;
using namespace std;

std::string IrcMessage::GetReturnName()
{
    if (command == "PRIVMSG" && params.size() > 0)
    {
        if (params[0][0] == '#')
            return params[0];
        else
            return prefix.substr(1, prefix.find('!') - 1);
    }
    else
        return "";
}

IrcClient::IrcClient(unique_ptr<ISocket> s, string nick) :
    socket_(move(s)),
    receivedFirstPing_(false)
{
    // Perform handshake
    ostringstream ss;

    ss << "NICK " << nick << "\r\n";
    socket_->Send(ss.str());
    ss.clear();

    ss << "USER " << nick << " localhost servername " << nick << "\r\n";
    socket_->Send(ss.str());
    ss.clear();
}

IrcClient::IrcClient(string host, string port, string nick) :
    socket_(make_unique<Socket>(host, port)),
    receivedFirstPing_(false)
{
    // Perform handshake
    ostringstream ss;
    
    ss << "NICK " << nick << "\r\n";
    socket_->Send(ss.str());
    ss.clear();

    ss << "USER " << nick << " localhost servername " << nick << "\r\n";
    socket_->Send(ss.str());
    ss.clear();
}

void IrcClient::Send(string message)
{
    // build in flood protection
    socket_->Send(message);
}

IrcMessage IrcClient::Receive()
{
    auto endline = remains_.find("\r\n");
    while (endline == string::npos)
    {
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        auto bytesRead = socket_->Receive(buffer, sizeof(buffer));
        if (bytesRead >= 0)
            remains_.append(buffer, bytesRead);
        else
        {
            // Socket error. OHSHI-
            return IrcMessage();
        }
        endline = remains_.find("\r\n");
    }

    string result = remains_.substr(0, endline);
    remains_ = remains_.substr(endline + 2);

    // If result is a ping, pong it
    auto pos = result.find("PING ");
    if (pos == 0)
    {
        ostringstream ss;
        ss << "PONG " << result.substr(5) << "\r\n";
        Send(ss.str());
        result = "";
        receivedFirstPing_ = true;
    }

    return ParseMessage(result);
}

IrcMessage IrcClient::ParseMessage(string m)
{
    IrcMessage message;
    if (m.length() == 0)
        return message;

    size_t start = 0;
    auto pos = m.find(' ', start);
    message.prefix = m.substr(start, pos - start);
    start = pos + 1;
    pos = m.find(' ', start);
    message.command = m.substr(start, pos - start);
    start = pos + 1;
    while (true)
    {
        if (m[start] == ':')
        {
            start ++; // Skip the ':'
            break;
        }

        pos = m.find(' ', start);
        if (pos == string::npos)
            break;
        message.params.push_back(m.substr(start, pos - start));
        start = pos + 1;
    }
    message.params.push_back(m.substr(start));
    return message;
}
