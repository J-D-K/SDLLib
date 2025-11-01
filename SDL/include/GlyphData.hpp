#pragma once
#include "Texture.hpp"

#include <cstdint>

namespace sdl::text
{
    // clang-format off
    struct GlyphData
    {
        uint16_t width{};
        uint16_t height{};
        int16_t advanceX{};
        int16_t top{};
        int16_t left{};
        sdl::SharedTexture texture{};
    };
    // clang-format on
}