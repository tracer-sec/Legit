#include "catch.hpp"
#include "HttpUtils.hpp"

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

    REQUIRE(result == "foo=bar&baz=123");
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

    string compare = 
        "--" + boundary + "\r\n" +
        "Content-Disposition: form-data; name=\"foo\"\r\n" +
        "\r\n" +
        "bar\r\n" +
        "--" + boundary + "\r\n" +
        "Content-Disposition: form-data; name=\"baz\"\r\n" +
        "\r\n" +
        "123\r\n" +
        "--" + boundary + "\r\n" +
        "Content-Disposition: form-data; name=\"f0\"; filename=\"original_f0.txt\"\r\n" +
        "Content-Type: application/octet-stream\r\n" +
        "\r\n" +
        "AAAAAAAAAAAAAAAA\r\n" +
        "--" + boundary + "\r\n" +
        "Content-Disposition: form-data; name=\"f1\"; filename=\"original_f1.txt\"\r\n" +
        "Content-Type: application/octet-stream\r\n" +
        "\r\n" +
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\r\n" +
        "--" + boundary + "--\r\n";

    REQUIRE(result == compare);
}

TEST_CASE("HttpUtils parse url")
{
    auto result = HttpUtils::SplitUrl("http://www.example.com/test/foo?bar=baz");

    REQUIRE(result[0] == "http://");
    REQUIRE(result[1] == "www.example.com");
    REQUIRE(result[2] == "/test/foo?bar=baz");
}
