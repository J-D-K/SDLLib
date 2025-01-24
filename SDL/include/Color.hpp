#pragma once
#include <cstdint>

namespace sdl
{
    /// @brief This isn't really a part of SDL. I just like handling RGBA this way.
    union Color
    {
            /// @brief RAW 32 bit pixel.
            uint32_t raw;

            /// @brief 32 bit pixel split into four to make accessing the values easier.
            uint8_t rgba[4];
    };
} // namespace sdl
