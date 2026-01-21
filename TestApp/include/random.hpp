#pragma once
#include <cstdlib>

/// @brief Generates a random number with max being the highest the number can be.
inline int generate_random(int max) noexcept { return std::rand() % max; }
