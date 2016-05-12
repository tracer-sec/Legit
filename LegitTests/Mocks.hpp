#pragma once

#include "Socket.hpp"
#include <sstream>

class Mocket : public Legit::ISocket
{
public:
    Mocket(std::string host, std::string port, 
        std::ostringstream &sendStream, std::string response = "");

    int Send(const char *buffer, size_t length) override;
    int Send(std::string s) override;
    int Receive(char *buffer, size_t length) override;
    void Close() override { }

    std::string ToString() override { return "Mocket"; }
    std::string GetError() override { return ""; }
    std::string GetHost() override { return host_; }

private:
    std::ostringstream &sent_;
    std::string host_;
    std::string response_;
};
