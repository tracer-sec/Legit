#include "catch.hpp"
#include "HttpUtils.hpp"
#include "Utils.hpp"

using namespace Legit;
using namespace std;

TEST_CASE("HttpUtils www form encoding")
{
    unordered_map<string, string> fields = {
        { "foo", "bar" },
        { "baz", "123" }
    };

    auto body = HttpUtils::CreateBody(fields);
    string result(body.begin(), body.end());

    vector<string> parts = Utils::Split(result, "&");

    REQUIRE(parts.size() == 2);
    REQUIRE(find(parts.begin(), parts.end(), "foo=bar") != parts.end());
    REQUIRE(find(parts.begin(), parts.end(), "baz=123") != parts.end());
}

TEST_CASE("HttpUtils multipart encoding")
{
    unordered_map<string, string> fields = {
        { "foo", "bar" },
        { "baz", "123" }
    };

    HttpFile f0("original_f0.txt", vector<char>(16, 65)); // a
    HttpFile f1("original_f1.txt", vector<char>(32, 66)); // b

    unordered_map<string, HttpFile> files = {
        { "f0", f0 },
        { "f1", f1 }
    };

    string boundary;
    auto body = HttpUtils::CreateBody(fields, files, boundary);
    string result(body.begin(), body.end());

    vector<string> parts = Utils::Split(result, "--" + boundary);

    REQUIRE(parts.size() == 6);
    REQUIRE(parts[0] == "");
    REQUIRE(parts[5] == "--\r\n");

    vector<string> compare = {
        "\r\n"
        "Content-Disposition: form-data; name=\"foo\"\r\n"
        "\r\n"
        "bar\r\n",

        "\r\n"
        "Content-Disposition: form-data; name=\"baz\"\r\n"
        "\r\n"
        "123\r\n",
        
        "\r\n"
        "Content-Disposition: form-data; name=\"f0\"; filename=\"original_f0.txt\"\r\n"
        "Content-Type: application/octet-stream\r\n"
        "\r\n"
        "AAAAAAAAAAAAAAAA\r\n",
        
        "\r\n"
        "Content-Disposition: form-data; name=\"f1\"; filename=\"original_f1.txt\"\r\n"
        "Content-Type: application/octet-stream\r\n"
        "\r\n"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\r\n"
    };

    for (string c : compare)
    {
        REQUIRE(find(parts.begin(), parts.end(), c) != parts.end());
    }
}

TEST_CASE("HttpUtils parse url")
{
    auto result = HttpUtils::SplitUrl("http://www.example.com/test/foo?bar=baz");

    REQUIRE(result[0] == "http://");
    REQUIRE(result[1] == "www.example.com");
    REQUIRE(result[2] == "");
    REQUIRE(result[3] == "/test/foo?bar=baz");
}

TEST_CASE("HttpUtils parse url 2")
{
    auto result = HttpUtils::SplitUrl("https://2600.london");

    REQUIRE(result[0] == "https://");
    REQUIRE(result[1] == "2600.london");
    REQUIRE(result[2] == "");
    REQUIRE(result[3] == "");
}

TEST_CASE("HttpUtils parse url with port")
{
    auto result = HttpUtils::SplitUrl("http://localhost:8080/test/foo");

    REQUIRE(result[0] == "http://");
    REQUIRE(result[1] == "localhost");
    REQUIRE(result[2] == "8080");
    REQUIRE(result[3] == "/test/foo");
}

TEST_CASE("HttpUtils url encoding")
{
    auto result0 = HttpUtils::UrlEncode("foo=bar&q=wtf & for+why?");
    REQUIRE(result0 == "foo%3Dbar%26q%3Dwtf%20%26%20for%2Bwhy%3F");

    auto result1 = HttpUtils::UrlEncode("http://www.example.com");
    REQUIRE(result1 == "http%3A%2F%2Fwww.example.com");
}

