#include "Random.hpp"

using namespace Legit;
using namespace std;

RandomGenerator::RandomGenerator()
{
    random_device r;
    engine_.seed(r());
}

RandomGenerator::RandomGenerator(unsigned long seed)
{
    Reseed(seed);
}

void RandomGenerator::Reseed(unsigned long seed)
{
    engine_.seed(seed);
}

int RandomGenerator::GetInt(int min, int max)
{
    uniform_int_distribution<int> uniform(min, max);
    return uniform(engine_);
}

float RandomGenerator::GetFloat(float min, float max)
{
    uniform_real_distribution<float> uniform(min, max);
    return uniform(engine_);
}

double RandomGenerator::GetDouble(double min, double max)
{
    uniform_real_distribution<double> uniform(min, max);
    return uniform(engine_);
}

string RandomGenerator::GetString(string pool, unsigned int length)
{
    string s;
    s.resize(length);
    for (unsigned int i = 0; i < s.length(); ++i)
    {
        int j = GetInt(0, pool.length() - 1);
        s[i] = pool[j];
    }
    return s;
}
