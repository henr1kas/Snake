#pragma once

#include <random>

namespace utils {
    inline std::random_device rd;
    inline std::mt19937 gen(rd());

    /* generates a random number [min, max] (both included) */
    inline std::int64_t random(const std::int64_t min, const std::int64_t max) {
        std::uniform_int_distribution<std::int64_t> dis(min, max);
        return dis(gen);
    }
}