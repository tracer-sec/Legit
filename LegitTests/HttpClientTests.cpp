#include "catch.hpp"
#include "HttpClient.hpp"
#include "Mocks.hpp"

#ifndef _WIN32
#include "Shims.hpp"
#endif

using namespace Legit;
using namespace std;

TEST_CASE("HttpClient valid GET request")
{
    ostringstream sent;
    //HttpClient client(make_unique<Mocket>("www.google.co.uk", "80", sent, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\n<html></html>"));
    HttpClient client(make_unique<Mocket>("www.google.co.uk", "80", sent));
    client.Get("/index.html");

    string result = sent.str();

    REQUIRE(result.find("GET /index.html HTTP/1.1\r\n") == 0);
    REQUIRE(result.find("\r\nHost: www.google.co.uk\r\n") != string::npos);
}

TEST_CASE("HttpClient test ParseResponse")
{
    ostringstream sent;
    HttpClient client(make_unique<Mocket>("www.google.co.uk", "80", sent, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\n<html></html>"));
    auto response = client.Get("/index.html");

    REQUIRE(response.statusCode == "HTTP/1.1 200 OK");
    REQUIRE(response.headers.size() == 1);
    REQUIRE(response.headers["Content-Length"] == "13");
    REQUIRE(response.body == "<html></html>");
}
