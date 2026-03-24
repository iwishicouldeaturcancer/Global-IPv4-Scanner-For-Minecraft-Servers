#pragma once
#include <random>
#include <string>

class IPGenerator {
    std::mt19937 rng;
    std::uniform_int_distribution<uint32_t> dist;

public:
    IPGenerator();
    std::string next();
};