#include "text.hpp"

#include "ResourceManager.hpp"
#include "error.hpp"
#include "sdl.hpp"

#include <algorithm>
#include <span>
#include <switch.h>
#include <unordered_map>
#include <unordered_set>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace
{
    /// @brief Special characters and they assigned colors.
    std::unordered_map<uint32_t, sdl::Color> s_specialCharacterMap;
} // namespace

// Finds the next available point to break at and returns a pointer to it.
static int find_next_breakpoint(std::string_view string)
{
    static const std::unordered_set<uint32_t> BREAKPOINTS = {L' ', L'　', L'/', L'_', L'-', L'。', L'、'};

    uint32_t codepoint{};
    int length = string.length();
    for (int i = 0; i < length;)
    {
        const uint8_t *nextPoint = reinterpret_cast<const uint8_t *>(&string[i]);
        const ssize_t count      = decode_utf8(&codepoint, nextPoint);
        if (count <= 0) { break; }
        i += count;

        const auto breakpoint = BREAKPOINTS.find(codepoint);
        if (breakpoint != BREAKPOINTS.end()) { return i; }
    }
    return length;
}

static inline bool process_special_characters(uint32_t codepoint, sdl::Color originalColor, sdl::Color &textColor)
{
    const auto isSpecial = s_specialCharacterMap.find(codepoint);
    if (isSpecial == s_specialCharacterMap.end()) { return false; }

    if (textColor.raw == isSpecial->second.raw) { textColor = originalColor; }
    else { textColor = isSpecial->second; }
    return true;
}

void sdl::text::render(sdl::SharedTexture &target,
                       int x,
                       int y,
                       int fontSize,
                       int wrapWidth,
                       sdl::Color color,
                       std::string_view text)
{
    if (!sdl::set_render_target(target)) { return; }

    // Need to preserve original coordinates and color.
    const int maxWidth      = x + wrapWidth;
    int workingX            = x;
    int workingY            = y;
    sdl::Color workingColor = color;

    sdl::text::SystemFont::resize(fontSize);

    const int length = text.length();
    for (int i = 0; i < length;)
    {
        std::string_view blockBegin = text.substr(i);

        const int nextBreak    = find_next_breakpoint(blockBegin);
        std::string_view block = text.substr(i, nextBreak);

        if (wrapWidth != sdl::text::NO_WRAP)
        {
            const int blockWidth = sdl::text::get_width(fontSize, block);
            if (workingX + blockWidth >= maxWidth)
            {
                workingX = x;
                workingY += fontSize + (fontSize / 3);
            }
        }

        int j{};
        int blockLength = block.length();
        uint32_t codepoint{};
        for (j = 0; j < blockLength;)
        {
            const uint8_t *unitBegin = reinterpret_cast<const uint8_t *>(&block[j]);
            const ssize_t count      = decode_utf8(&codepoint, unitBegin);
            if (count <= 0) { break; }

            const bool isSpecial = process_special_characters(codepoint, color, workingColor);
            const bool isBreak   = codepoint == L'\n';
            if (isSpecial)
            {
                ++j;
                continue;
            }
            else if (isBreak)
            {
                ++j;
                workingX = x;
                workingY += fontSize + (fontSize / 3);
                continue;
            }

            auto glyph = sdl::text::SystemFont::find_load_glyph(codepoint);
            if (!glyph.has_value()) { return; }

            const auto &data = glyph->get();
            if (codepoint == L' ') { workingX += data.advanceX; }
            else
            {
                const int renderX = workingX + data.left;
                const int renderY = workingY + (fontSize - data.top);

                data.texture->set_color_mod(workingColor);
                data.texture->render(target, renderX, renderY);
                workingX += data.advanceX;
            }
            j += count;
        }
        i += j;
    }
}

int sdl::text::get_width(int fontSize, std::string_view text)
{
    uint32_t codepoint{};
    int width{};
    int length           = text.length();
    sdl::Color tempColor = {0};
    sdl::text::SystemFont::resize(fontSize);

    for (int i = 0; i < length;)
    {
        const uint8_t *unitBegin = reinterpret_cast<const uint8_t *>(&text[i]);
        ssize_t count            = decode_utf8(&codepoint, unitBegin);
        if (count <= 0) { break; }

        const bool isSpecial = process_special_characters(codepoint, tempColor, tempColor);
        if (isSpecial)
        {
            ++i;
            continue;
        }

        auto glyph = sdl::text::SystemFont::find_load_glyph(codepoint);
        if (!glyph.has_value())
        {
            i += count;
            continue;
        }

        width += glyph->get().advanceX;
        i += count;
    }
    return width;
}

void sdl::text::add_color_character(uint32_t codepoint, sdl::Color color) { s_specialCharacterMap[codepoint] = color; }
