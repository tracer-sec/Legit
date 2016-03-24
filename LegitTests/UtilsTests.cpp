#include "catch.hpp"
#include "Utils.h"

using namespace std;

TEST_CASE("basic string search and replace")
{
    auto result = Utils::FindAndReplace(L"This is a test, mate", L" test,", L" successful test,");
    REQUIRE(result == L"This is a successful test, mate");
}

TEST_CASE("basic string search and replace missing")
{
    auto result = Utils::FindAndReplace(L"This is a test, mate", L" successful ", L" failing ");
    REQUIRE(result == L"This is a test, mate");
}

TEST_CASE("multiple matches string replace")
{
    auto result = Utils::FindAndReplace(L"111 test 222 test 333 test 444", L"test", L"hurray");
    REQUIRE(result == L"111 hurray 222 hurray 333 hurray 444");
}

TEST_CASE("string replace at start")
{
    auto result = Utils::FindAndReplace(L"replacing at the end", L"end", L"END");
    REQUIRE(result == L"replacing at the END");
}

TEST_CASE("string replace at end")
{
    auto result = Utils::FindAndReplace(L"replacing at the start", L"replacing", L"replaced");
    REQUIRE(result == L"replaced at the start");
}

TEST_CASE("string trim")
{
    auto result = Utils::Trim(" \t    \n\r\n  this is\r \na\t test\t\t\t\n\r\n  \n");
    REQUIRE(result == "this is\r \na\t test");
}

TEST_CASE("basic hex test")
{
    vector<char> data = { 'a', 'b', 'C', 'D' };
    auto result = Utils::Hex(data);
    REQUIRE(result == "61624344");
}

TEST_CASE("basic hex test 2")
{
    char data[] = { 0x12, 0x34, 0x56, 0x78 };
    auto result = Utils::Hex(data, sizeof(data));
    REQUIRE(result == "12345678");
}


