#pragma once

#include <random>

static const std::string ALPHA = "abcdefghijklmnopqrstuvwxyz";
static const std::string ALPHA_MIXED = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const std::string ALPHA_NUMERIC = "abcdefghijklmnopqrstuvwxyz0123456789";
static const std::string ALPHA_NUMERIC_MIXED = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

class RandomGenerator
{
public:
    RandomGenerator(unsigned long seed);

    void Reseed(unsigned long seed);
    int GetInt(int min, int max);
    float GetFloat(float min, float max);
    double GetDouble(double min, double max);

    std::string GetString(std::string pool, unsigned int length);

private:
    std::mt19937 engine_;
};
