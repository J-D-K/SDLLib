#pragma once
#include <cstdint>

// This isn't really part of SDL. Just my own way of handling this stuff.
namespace sdl
{
    union Color
    {
            // Not sure what else to name these...
            uint32_t raw;
            // This is backwards from how you'd think it is.
            uint8_t rgba[4];
    };
} // namespace sdl
