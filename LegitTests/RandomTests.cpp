#include "catch.hpp"
#include "Random.hpp"

TEST_CASE("random seed test")
{
    auto r0 = RandomGenerator(100);
    auto int0 = r0.GetInt(0, 8192);
    auto float0 = r0.GetFloat(0, 8192);
    auto double0 = r0.GetDouble(0, 8192);
    auto string0 = r0.GetString(ALPHA_NUMERIC_MIXED, 32);

    r0.Reseed(100);
    auto int1 = r0.GetInt(0, 8192);
    auto float1 = r0.GetFloat(0, 8192);
    auto double1 = r0.GetDouble(0, 8192);
    auto string1 = r0.GetString(ALPHA_NUMERIC_MIXED, 32);

    REQUIRE(int0 == int1);
    REQUIRE(float0 == float1);
    REQUIRE(double0 == double1);
    REQUIRE(string0 == string1);
}
