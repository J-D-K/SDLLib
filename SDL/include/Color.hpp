#pragma once
#include <cstdint>

namespace sdl
{
    /// @brief This isn't really a part of SDL. I just like handling RGBA this way.
    // clang-format off
    union Color
    {
        /// @brief RAW 32 bit pixel.
        uint32_t raw;
        /// @brief 32 bit pixel split into four to make accessing the values easier.
        uint8_t rgba[4];
    };
    // clang-format on

    /// @brief Everything in here makes these easier to work with.
    namespace color
    {
        inline uint8_t get_red(sdl::Color color) { return color.rgba[3]; }
        inline uint8_t get_green(sdl::Color color) { return color.rgba[2]; }
        inline uint8_t get_blue(sdl::Color color) { return color.rgba[1]; }
        inline uint8_t get_alpha(sdl::Color color) { return color.rgba[0]; }

        inline void set_red(sdl::Color &color, uint8_t red) { color.rgba[3] = red; }
        inline void set_green(sdl::Color &color, uint8_t green) { color.rgba[2] = green; }
        inline void set_blue(sdl::Color &color, uint8_t blue) { color.rgba[1] = blue; }
        inline void set_alpha(sdl::Color &color, uint8_t alpha) { color.rgba[0] = alpha; }

        inline sdl::Color create_color(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha)
        {
            return {(red << 24 & 0xFF000000) | (green << 16 & 0xFF0000) | (blue << 8 & 0xFF00) | (alpha & 0xFF)};
        }
    }
} // namespace sdl
