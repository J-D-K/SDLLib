#include "Font.hpp"

#include <filesystem>
#include <fstream>

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

void sdl2::Font::render_text(int x, int y, std::string_view text) {}