#include "catch.hpp"

#include "Utils.h"

TEST_CASE("im_in_the_mood_for_testing")
{
    auto result = Utils::FindAndReplace(L"This is a test, mate", L" test,", L" successful test,");
    REQUIRE(result == L"This is a successful test, mate");
}
