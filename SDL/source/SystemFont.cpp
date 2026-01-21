#include "SystemFont.hpp"

#include <fstream>

//                      ---- Construction ----

sdl2::SystemFont::SystemFont(int pixelSize)
{
    // Set pixel size.
    m_pixelSize = pixelSize;

    // Grab reference to font array.
    const auto &plFontArray = sm_plService.m_sharedFonts;

    // Loop through, load, and size.
    size_t faceIndex{};
    for (const PlFontData &fontData : plFontArray)
    {
        // This makes the next thing easier to read.
        const FT_Byte *fontBinary = reinterpret_cast<const FT_Byte *>(fontData.address);

        // Init face.
        const FT_Error faceError =
            FT_New_Memory_Face(sm_freetype.m_library, fontBinary, fontData.size, 0, &m_fontFaces[faceIndex]);
        if (faceError != 0) { continue; }

        // Set the pixel size.
        FT_Set_Pixel_Sizes(m_fontFaces[faceIndex++], 0, m_pixelSize);
    }
}

sdl2::SystemFont::~SystemFont()
{
    // Loop and free faces.
    for (FT_Face fontFace : m_fontFaces) { FT_Done_Face(fontFace); }
}

//                      ---- Private Functions ----

OptionalReference<sdl2::Font::GlyphData> sdl2::SystemFont::find_load_glyph(uint32_t codepoint)
{
    std::ofstream sysFontDebug{"sdmc:/sysfont.txt"};

    // Search first.
    const auto findGlyph = m_cacheMap.find(codepoint);
    if (findGlyph != m_cacheMap.end()) { return findGlyph->second; }

    // Loop and try to find the index.
    FT_Face fontFace{};
    FT_UInt charIndex{};
    for (FT_Face currentFace : m_fontFaces)
    {
        if (!currentFace) { continue; }

        // Try to get the index. If it's found, break the loop.
        charIndex = FT_Get_Char_Index(currentFace, codepoint);
        if (charIndex != 0)
        {
            sysFontDebug << "Character found?" << std::endl;
            fontFace = currentFace;
            break;
        }
    }

    // If the character index is still 0 here, bail.
    if (charIndex == 0) { return std::nullopt; }
    sysFontDebug << "charIndex" << charIndex << std::endl;

    // Load and render the glyph.
    const FT_Error loadError = FT_Load_Glyph(fontFace, charIndex, FT_LOAD_RENDER);
    if (loadError != 0) { return std::nullopt; }
    sysFontDebug << "Character rendered." << std::endl;

    // Make things easier to read and type.
    const FT_GlyphSlot glyphSlot = fontFace->glyph;
    const FT_Bitmap glyphBitmap  = glyphSlot->bitmap;

    // Convert.
    sdl2::SharedTexture glyphTexture = Font::convert_glyph_to_texture(codepoint, glyphBitmap);
    sysFontDebug << "Texture" << std::endl;

    // Data for cache.
    const Font::GlyphData cacheData = {.advanceX = static_cast<int16_t>(glyphSlot->advance.x >> 6),
                                       .top      = static_cast<int16_t>(glyphSlot->bitmap_top),
                                       .left     = static_cast<int16_t>(glyphSlot->bitmap_left),
                                       .texture  = glyphTexture};

    // Map is weird and returns this as a pair?
    const auto emplacePair = m_cacheMap.try_emplace(codepoint, cacheData);
    if (!emplacePair.second) { return std::nullopt; }
    sysFontDebug << "Emplace" << std::endl;

    // Finally return.
    return m_cacheMap.at(codepoint);
}