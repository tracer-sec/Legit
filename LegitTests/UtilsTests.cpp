#include "catch.hpp"
#include "Utils.h"

using namespace std;

TEST_CASE("basic_string_search_and_replace")
{
    auto result = Utils::FindAndReplace(L"This is a test, mate", L" test,", L" successful test,");
    REQUIRE(result == L"This is a successful test, mate");
}

TEST_CASE("basic_string_search_and_replace_missing")
{
    auto result = Utils::FindAndReplace(L"This is a test, mate", L" successful ", L" failing ");
    REQUIRE(result == L"This is a test, mate");
}

TEST_CASE("basic_hex_test")
{
    vector<char> data = { 'a', 'b', 'C', 'D' };
    auto result = Utils::Hex(data);
    REQUIRE(result == "61624344");
}

