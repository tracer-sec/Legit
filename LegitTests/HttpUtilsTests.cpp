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

    REQUIRE(result == "");
}
