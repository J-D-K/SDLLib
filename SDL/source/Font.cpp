#include "Font.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <span>
#include <switch.h>

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

void sdl2::Font::render_text(int x, int y, std::string_view text)
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
        if (unitCount <= 0) { break; } // Break the loop since something funky happened.

        // Handle line breaks.
        if (codepoint == L'\n')
        {
            x = originalX;
            y += m_pixelSize + (m_pixelSize / 4);
            continue;
        }

        // Try to find the glyph.
        const auto getGlyph = Font::find_load_glyph(codepoint);
        // If the optional is empty, just continue the loop. No glyph was found.
        if (!getGlyph.has_value())
        {
            i += unitCount;
            continue;
        }

        // Data reference to make things easier.
        const Font::GlyphData &glyphData = getGlyph->get();

        // Only render the glyph if it's not space.
        if (codepoint != L' ')
        {
            // Render coordinates.
            const int renderX = x + (glyphData.left);
            const int renderY = y + (m_pixelSize - glyphData.top);

            glyphData.texture->render(renderX, renderY);
        }

        // Move our rendering point.
        x += glyphData.advanceX;

        // Increase i.
        i += unitCount;
    }
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
    // static constexpr uint32_t BASE_PIXEL_COLOR = 0x00000000;

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