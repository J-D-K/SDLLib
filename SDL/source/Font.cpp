#include "Font.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <span>
#include <switch.h>

namespace
{
    constexpr uint32_t LINE_BREAK = L'\n';
}

//                      ---- Construction ----

sdl2::Font::Font(std::string_view fontPath, int pixelSize)
    : m_pixelSize{pixelSize}
{
    // Attempt to get the font size.
    const size_t fontSize = std::filesystem::file_size(fontPath);
    if (fontSize == 0) { return; }

    // Allocate buffer.
    m_fontBuffer = std::make_unique<FT_Byte[]>(fontSize);
    if (!m_fontBuffer) { return; }

    // Read the file.
    std::ifstream fontFile{fontPath.data(), std::ios::binary};
    if (!fontFile.is_open()) { return; }

    fontFile.read(reinterpret_cast<char *>(m_fontBuffer.get()), fontSize);
    if (fontFile.gcount() != static_cast<int64_t>(fontSize)) { return; }

    // Create the freetype memory face.
    FT_Error ftError = FT_New_Memory_Face(sm_freetype.m_library, m_fontBuffer.get(), fontSize, 0, &m_fontFace);
    if (ftError != 0) { return; }

    // Set the pixel sizes.
    ftError = FT_Set_Pixel_Sizes(m_fontFace, 0, m_pixelSize);
    if (ftError != 0) { return; }

    m_isInitialized = true;
}

sdl2::Font::~Font()
{
    if (!m_fontFace) { return; }

    FT_Done_Face(m_fontFace);
}

//                      ---- Public Functions ----

void sdl2::Font::render_text(int x, int y, SDL_Color color, std::string_view text)
{
    // Need to store this for line breaks.
    const int originalX = x;

    const size_t length = text.length();
    for (size_t i = 0; i < length;)
    {
        // Cast to uint8_t since that's what libnx prefers.
        const uint8_t *string = reinterpret_cast<const uint8_t *>(&text[i]);

        // Decode the codepoint.
        uint32_t codepoint{};
        ssize_t unitCount = decode_utf8(&codepoint, string);
        i += unitCount <= 0 ? 0 : unitCount;
        if (unitCount <= 0) { break; } // Break the loop since something funky happened.
        else if (codepoint == LINE_BREAK)
        {
            x = originalX;
            y += m_pixelSize * 1.25;
            continue;
        }

        // Try to find the glyph.
        const auto getGlyph = find_load_glyph(codepoint);
        if (!getGlyph.has_value()) { continue; }

        // Data reference to make things easier.
        const Font::GlyphData &glyphData = getGlyph->get();
        if (codepoint != L' ')
        {
            // Render coordinates.
            const int renderX = x + (glyphData.left);
            const int renderY = y + (m_pixelSize - glyphData.top);

            // Set render color.
            glyphData.texture->set_color_mod(color);
            glyphData.texture->render(renderX, renderY);
        }

        // Move our rendering point.
        x += glyphData.advanceX;
    }
}

void sdl2::Font::render_text_wrapped(int x, int y, SDL_Color color, int maxWidth, std::string_view text)
{
    // Store this for later.
    const int originalX = x;

    // Adjusted max position.
    const int maxPosition = x + maxWidth;

    // Width of the current line we're rendering.
    int currentWidth{};

    // For the loop.
    const int textLength = text.length();

    // This is a laziness I mean repetition thing.
    auto break_line = [&]()
    {
        x = originalX;
        y += m_pixelSize * 1.25;
        currentWidth = 0;
    };

    for (int i = 0; i < textLength;)
    {
        // Find the next breakpoint.
        const size_t nextBreakpoint = Font::find_next_breakpoint(text.substr(i));

        // Store the substring as a word.
        const std::string_view word = text.substr(i, nextBreakpoint);

        // Grab the width and see if we need to break the line.
        const int wordWidth = Font::get_text_width(word);
        if (x + wordWidth >= maxPosition) { break_line(); }

        // Word render loop.
        const int wordLength = word.length();
        for (int j = 0; j < wordLength;)
        {
            uint32_t codepoint{};
            const uint8_t *point    = reinterpret_cast<const uint8_t *>(&word[j]);
            const ssize_t unitCount = decode_utf8(&codepoint, point);
            j += unitCount <= 0 ? 0 : unitCount;
            if (unitCount <= 0) { break; }
            else if (codepoint == LINE_BREAK)
            {
                break_line();
                continue;
            }

            const auto getGlyph = find_load_glyph(codepoint);
            if (!getGlyph.has_value()) { continue; }

            Font::GlyphData &glyphData = getGlyph->get();
            if (codepoint != L' ')
            {
                const int renderX = x + glyphData.left;
                const int renderY = y + (m_pixelSize - glyphData.top);

                glyphData.texture->set_color_mod(color);
                glyphData.texture->render(renderX, renderY);
            }

            x += glyphData.advanceX;
        }

        // Realign.
        i += wordLength;
        currentWidth += wordWidth;
    }
}

int sdl2::Font::get_text_width(std::string_view text)
{
    // This is what we're returning.
    int textWidth{};

    const int textLength = text.length();
    for (int i = 0; i < textLength;)
    {
        uint32_t codepoint{};
        const uint8_t *point    = reinterpret_cast<const uint8_t *>(&text[i]);
        const ssize_t unitCount = decode_utf8(&codepoint, point);

        i += unitCount <= 0 ? 0 : unitCount;
        if (unitCount <= 0) { return textWidth; }
        else if (codepoint == LINE_BREAK) { continue; } // Ignore line breaks.

        // Load glyph.
        const auto getGlyph = find_load_glyph(codepoint);
        if (!getGlyph.has_value()) { continue; }

        const Font::GlyphData &glyphData = getGlyph->get();
        textWidth += glyphData.advanceX;
    }

    return textWidth;
}

//                      ---- Public, static functions ----

void sdl2::Font::add_break_point(uint32_t codepoint) { sm_breakPoints.push_back(codepoint); }

void sdl2::Font::add_break_points(std::initializer_list<uint32_t> pointList)
{
    for (uint32_t point : pointList) { sm_breakPoints.push_back(point); }
}

void sdl2::Font::add_break_points(std::span<uint32_t> pointSpan)
{
    for (uint32_t point : pointSpan) { sm_breakPoints.push_back(point); }
}

void sdl2::Font::add_color_point(uint32_t codepoint, SDL_Color color)
{
    sm_colorPoints.push_back(std::make_pair(codepoint, color));
}

void sdl2::Font::add_color_points(std::initializer_list<std::pair<uint32_t, SDL_Color>> pointList)
{
    for (auto pair : pointList) { sm_colorPoints.push_back(pair); }
}

void sdl2::Font::add_color_points(std::span<std::pair<uint32_t, SDL_Color>> pointSpan)
{
    for (const auto pair : pointSpan) { sm_colorPoints.push_back(pair); }
}

//                      ---- Protected Functions ----

OptionalReference<sdl2::Font::GlyphData> sdl2::Font::find_load_glyph(uint32_t codepoint)
{
    // Search for the glyph first.
    const auto findGlyph = m_cacheMap.find(codepoint);
    if (findGlyph != m_cacheMap.end()) { return findGlyph->second; }

    // Get the character index.
    FT_UInt glyphIndex = FT_Get_Char_Index(m_fontFace, codepoint);
    if (glyphIndex <= 0) { return std::nullopt; }

    // Load and render the glyph.
    FT_Error ftError = FT_Load_Glyph(m_fontFace, glyphIndex, FT_LOAD_RENDER);
    if (ftError != 0) { return std::nullopt; }

    // Make things easier to type and read.
    const FT_GlyphSlot glyphSlot = m_fontFace->glyph;
    const FT_Bitmap glyphBitmap  = glyphSlot->bitmap;

    // Convert to texture.
    sdl2::SharedTexture glyphTexture = Font::convert_glyph_to_texture(codepoint, glyphBitmap);

    // Cache data.
    const Font::GlyphData cacheData = {.advanceX = static_cast<int16_t>(glyphSlot->advance.x >> 6),
                                       .top      = static_cast<int16_t>(glyphSlot->bitmap_top),
                                       .left     = static_cast<int16_t>(glyphSlot->bitmap_left),
                                       .texture  = glyphTexture};

    // Map is weird and returns this as a pair?
    const auto emplacePair = m_cacheMap.try_emplace(codepoint, cacheData);
    if (!emplacePair.second) { return std::nullopt; }

    // Finally return.
    return m_cacheMap.at(codepoint);
}

sdl2::SharedTexture sdl2::Font::convert_glyph_to_texture(uint32_t codepoint, const FT_Bitmap glyphBitmap)
{
    // Base pixel color for constructing pixels.
    static constexpr uint32_t BASE_PIXEL_COLOR = 0xFFFFFF00;

    // This makes things easier to read later.
    const size_t bitmapSize = glyphBitmap.width * glyphBitmap.rows;

    // Surface to construct on.
    sdl2::Surface glyphSurface = sdl2::surface::create_rgb(glyphBitmap.width, glyphBitmap.rows);
    if (!glyphSurface) { return nullptr; } // To do: Something better here.

    // Spans for constructing pixels.
    std::span<uint32_t> surfacePixels{reinterpret_cast<uint32_t *>(glyphSurface->pixels), bitmapSize};
    std::span<const uint8_t> bitmapPixels{glyphBitmap.buffer, bitmapSize};

    // Loop and construct glyph.
    for (size_t i = 0; i < bitmapSize; i++) { surfacePixels[i] = BASE_PIXEL_COLOR | bitmapPixels[i]; }

    // Name for the glyph.
    const std::string glyphName = std::format("{:08x} - {}", codepoint, m_pixelSize);
    return sdl2::TextureManager::create_load_resource(glyphName, glyphSurface);
}

//                      ---- Private Functions ----

size_t sdl2::Font::find_next_breakpoint(std::string_view string)
{
    const int stringLength = string.length();
    for (int i = 0; i < stringLength;)
    {
        uint32_t codepoint{};
        const uint8_t *nextPoint = reinterpret_cast<const uint8_t *>(&string[i]);

        const ssize_t unitCount = decode_utf8(&codepoint, nextPoint);
        if (unitCount <= 0) { return string.npos; }
        else if (Font::is_breakpoint(codepoint)) { return i + unitCount; }

        i += unitCount;
    }
    return string.npos;
}

bool sdl2::Font::is_breakpoint(uint32_t codepoint) const noexcept
{
    auto match = [=](const uint32_t point) { return point == codepoint; };
    return std::find_if(sm_breakPoints.begin(), sm_breakPoints.end(), match) != sm_breakPoints.end();
}

bool sdl2::Font::is_color_point(uint32_t codepoint) const noexcept
{
    auto match = [=](const auto &pair) { return pair.first == codepoint; };
    return std::find_if(sm_colorPoints.begin(), sm_colorPoints.end(), match) != sm_colorPoints.end();
}

SDL_Color sdl2::Font::get_point_color(uint32_t codepoint) const noexcept
{
    auto match    = [=](const auto &pair) { return pair.first == codepoint; };
    auto findPair = std::find_if(sm_colorPoints.begin(), sm_colorPoints.end(), match);
    if (findPair == sm_colorPoints.end()) { return static_cast<SDL_Color>(0); }

    return findPair->second;
}