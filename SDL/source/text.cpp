#include "text.hpp"

#include "TextureManager.hpp"
#include "error.hpp"
#include "sdl.hpp"

#include <algorithm>
#include <array>
#include <ft2build.h>
#include <map>
#include <string>
#include <switch.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace
{
    /// @brief Library used to load the shared system font.
    FT_Library s_ftLib{};

    /// @brief Faces for each font of the system.
    FT_Face s_ftFaces[PlSharedFontType_Total]{};

    /// @brief Number of fonts successfully loaded.
    int32_t s_totalFonts = 0;

    // This struct holds the data needed to render each glyph.
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

    /// @brief Font glyph cache map.
    std::map<std::pair<uint32_t, int>, GlyphData> s_fontCacheMap;

    /// @brief Special characters and they assigned colors.
    std::unordered_map<uint32_t, sdl::Color> s_specialCharacterMap;

    /// @brief Buffer sizes for render.
    constexpr int VA_BUFFER_SIZE   = 0x1000;
    constexpr int WORD_BUFFER_SIZE = 0x100;

    /// @brief Characters to break lines at.
    constexpr std::array<uint32_t, 7> BREAKPOINT_CHARS = {L' ', L'　', L'/', L'_', L'-', L'。', L'、'};
} // namespace

// Resizes Freetype faces to fontSize in pixes.
static bool resize_font(int fontSize)
{
    const FT_UInt pixelSize = static_cast<FT_UInt>(fontSize);

    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        if (sdl::error::freetype(FT_Set_Pixel_Sizes(s_ftFaces[i], 0, pixelSize))) { return false; }
    }
    return true;
}

// Finds the next available point to break at and returns a pointer to it.
static int find_next_breakpoint(std::string_view string)
{
    uint32_t codepoint{};
    int length = string.length();
    for (int i = 0; i < length;)
    {
        const uint8_t *nextPoint = reinterpret_cast<const uint8_t *>(&string[i]);
        const ssize_t count      = decode_utf8(&codepoint, nextPoint);
        if (count <= 0) { break; }
        i += count;

        const auto breakpoint = std::find(BREAKPOINT_CHARS.begin(), BREAKPOINT_CHARS.end(), codepoint);
        if (breakpoint != BREAKPOINT_CHARS.end()) { return i; };
    }
    return length;
}

static GlyphData *load_glyph(uint32_t codepoint, int fontSize)
{
    const auto cachePair = std::make_pair(codepoint, fontSize);
    {
        const auto findPair = s_fontCacheMap.find(cachePair);
        if (findPair != s_fontCacheMap.end()) { return &s_fontCacheMap[cachePair]; }
    }

    FT_GlyphSlot glyphSlot{};
    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        const FT_UInt pointIndex = FT_Get_Char_Index(s_ftFaces[i], codepoint);
        FT_Error ftError         = FT_Load_Glyph(s_ftFaces[i], pointIndex, FT_LOAD_RENDER);
        if (pointIndex != 0 && ftError == 0)
        {
            glyphSlot = s_ftFaces[i]->glyph;
            break;
        }
    }
    if (!glyphSlot) { return nullptr; }

    FT_Bitmap bitmap      = glyphSlot->bitmap;
    const uint16_t width  = bitmap.width;
    const uint16_t height = bitmap.rows;
    sdl::Surface surface  = sdl::surface::create_rgb(width, height);
    if (!surface) { return nullptr; }

    const int bitmapSize       = width * height;
    const uint8_t *glyphBuffer = bitmap.buffer;
    uint32_t *surfaceBuffer    = reinterpret_cast<uint32_t *>(surface->pixels);
    for (int i = 0; i < bitmapSize; i++) { *surfaceBuffer++ = 0xFFFFFF00 | *glyphBuffer++; }

    const int16_t advanceX      = glyphSlot->advance.x >> 6;
    const int16_t top           = glyphSlot->bitmap_top;
    const int16_t left          = glyphSlot->bitmap_left;
    const std::string cacheName = std::to_string(codepoint) + "_" + std::to_string(fontSize);
    sdl::SharedTexture texture  = sdl::TextureManager::create_load_texture(cacheName, surface);
    s_fontCacheMap[cachePair]   = {width, height, advanceX, top, left, texture};

    return &s_fontCacheMap[cachePair];
}

static inline bool process_special_characters(uint32_t codepoint, sdl::Color originalColor, sdl::Color &textColor)
{
    const auto isSpecial = s_specialCharacterMap.find(codepoint);
    if (isSpecial == s_specialCharacterMap.end()) { return false; }

    if (textColor.raw == isSpecial->second.raw) { textColor = originalColor; }
    else { textColor = isSpecial->second; }
    return true;
}

bool sdl::text::initialize()
{
    if (sdl::error::libnx(plInitialize(PlServiceType_User))) { return false; }
    if (sdl::error::libnx(setInitialize())) { return false; }
    if (sdl::error::freetype(FT_Init_FreeType(&s_ftLib))) { return false; }

    uint64_t languageCode{};
    if (sdl::error::libnx(setGetLanguageCode(&languageCode))) { return false; }
    setExit();

    PlFontData sharedFont[PlSharedFontType_Total]{};
    if (sdl::error::libnx(plGetSharedFont(languageCode, sharedFont, PlSharedFontType_Total, &s_totalFonts))) { return false; }

    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        const FT_Byte *address = reinterpret_cast<const FT_Byte *>(sharedFont[i].address);
        const FT_Long size     = sharedFont[i].size;

        if (sdl::error::freetype(FT_New_Memory_Face(s_ftLib, address, size, 0, &s_ftFaces[i]))) { return false; }
    }
    return true;
}

void sdl::text::exit()
{
    for (int32_t i = 0; i < s_totalFonts; i++) { FT_Done_Face(s_ftFaces[i]); }
    FT_Done_FreeType(s_ftLib);
    plExit();
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
    int workingX            = x;
    int workingY            = y;
    sdl::Color workingColor = color;

    resize_font(fontSize);

    int length = text.length();
    for (int i = 0; i < length;)
    {
        std::string_view blockBegin = text.substr(i);

        const int nextBreak    = find_next_breakpoint(blockBegin);
        std::string_view block = text.substr(i, nextBreak);

        if (wrapWidth != sdl::text::NO_WRAP)
        {
            const int blockWidth = sdl::text::get_width(fontSize, block);
            if (workingX + blockWidth >= wrapWidth)
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
            ssize_t count            = decode_utf8(&codepoint, unitBegin);
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

            GlyphData *glyph = load_glyph(codepoint, fontSize);
            if (!glyph) { continue; }

            if (codepoint == L' ') { workingX += glyph->advanceX; }
            else
            {
                const int renderX = workingX + glyph->left;
                const int renderY = workingY + (fontSize - glyph->top);

                glyph->texture->set_color_mod(workingColor);
                glyph->texture->render(target, renderX, renderY);
                workingX += glyph->advanceX;
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
    resize_font(fontSize);

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

        GlyphData *glyph = load_glyph(codepoint, fontSize);
        if (!glyph) { continue; }

        width += glyph->advanceX;
        i += count;
    }
    return width;
}

void sdl::text::add_color_character(uint32_t codepoint, sdl::Color color) { s_specialCharacterMap[codepoint] = color; }
