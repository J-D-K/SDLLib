#include "SystemFont.hpp"

#include "error.hpp"
#include "sdl.hpp"

#include <span>

//                      ---- Public functions ----

bool sdl::text::SystemFont::initialize()
{
    // Init the services needed.
    const bool plError  = error::libnx(plInitialize(PlServiceType_User));
    const bool setError = error::libnx(setInitialize());
    if (plError || setError) { return false; }

    // Freetype library.
    const bool initError = error::freetype(FT_Init_FreeType(&sm_ftLib));
    if (initError) { return false; }

    // Get the language code.
    uint64_t languageCode{};
    const bool codeError = error::libnx(setGetLanguageCode(&languageCode));
    if (codeError) { return false; }

    // set is no longer needed.
    setExit();

    // Load the data for the shared fonts.
    int32_t total{};
    std::array<PlFontData, PlSharedFontType_Total> fontData{};
    const bool fontError = error::libnx(plGetSharedFont(languageCode, fontData.data(), PlSharedFontType_Total, &total));
    if (fontError) { return false; }

    for (int32_t i = 0; i < total; i++)
    {
        // Cast these to make them easier to work with.
        const FT_Byte *address = reinterpret_cast<const FT_Byte *>(fontData[i].address);
        const FT_Long size     = fontData[i].size;

        const bool faceError = sdl::error::freetype(FT_New_Memory_Face(sm_ftLib, address, size, 0, &sm_faces[i]));
        if (faceError) { return false; }
    }

    return true;
}

void sdl::text::SystemFont::exit()
{
    // Loop and free the faces in use.
    for (FT_Face face : sm_faces)
    {
        if (!face) { continue; }

        FT_Done_Face(face);
    }

    // Free the library.
    FT_Done_FreeType(sm_ftLib);

    // Exit the service.
    plExit();
}

void sdl::text::SystemFont::resize(int fontSize) noexcept
{
    if (sm_fontSize == fontSize) { return; }

    sm_fontSize = fontSize;
    for (FT_Face face : sm_faces) { FT_Set_Pixel_Sizes(face, 0, fontSize); }
}

sdl::text::OptionalGlyph sdl::text::SystemFont::find_load_glyph(uint32_t codepoint)
{
    // If it's already loaded with the current font size, find it and return it.
    const auto mapPair  = std::make_pair(sm_fontSize, codepoint);
    const auto findPair = sm_glyphCache.find(mapPair);
    if (findPair != sm_glyphCache.end()) { return findPair->second; }

    // Attempt to load it from the font.
    FT_GlyphSlot glyphSlot = SystemFont::load_glyph(codepoint);
    if (!glyphSlot) { return std::nullopt; }

    // Convert it to a texture.
    sdl::SharedTexture glyphTexture = SystemFont::convert_slot_to_texture(glyphSlot);
    if (!glyphTexture) { return std::nullopt; }

    const uint16_t width   = glyphSlot->bitmap.width;
    const uint16_t height  = glyphSlot->bitmap.rows;
    const int16_t advanceX = glyphSlot->advance.x >> 6;
    const int16_t top      = glyphSlot->bitmap_top;
    const int16_t left     = glyphSlot->bitmap_left;
    sm_glyphCache[mapPair] = {width, height, advanceX, top, left, glyphTexture};

    return sm_glyphCache.at(mapPair);
}

//                      ---- Private functions ----

FT_GlyphSlot sdl::text::SystemFont::load_glyph(uint32_t codepoint)
{
    // Loop through all of the faces to find the glyph we need.
    for (FT_Face face : sm_faces)
    {
        if (!face) { continue; }

        const FT_UInt index = FT_Get_Char_Index(face, codepoint);
        FT_Error loadError  = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
        if (index != 0 && loadError == 0) { return face->glyph; }
    }

    return nullptr;
}

sdl::SharedTexture sdl::text::SystemFont::convert_slot_to_texture(FT_GlyphSlot slot)
{
    // This is the base white we're using. The alpha is blank and replaced with the freetype bitmap values.
    static constexpr uint32_t BASE_COLOR = 0xFFFFFF00;

    // This is just to ID glyphs for the manager.
    static int glyphID{};

    // We need this stuff.
    const size_t bitmapWidth  = slot->bitmap.width;
    const size_t bitmapHeight = slot->bitmap.rows;
    const size_t bitmapSize   = bitmapWidth * bitmapHeight;

    // Temporary surface we're blitting to.
    sdl::Surface tempSurface = sdl::surface::create_rgb(bitmapWidth, bitmapHeight);
    if (!tempSurface) { return nullptr; }

    // Spans for blitting raw pixels.
    const std::span<uint8_t> bitmapSpan{slot->bitmap.buffer, bitmapSize};
    const std::span<uint32_t> surfacePixels{reinterpret_cast<uint32_t *>(tempSurface.get()->pixels), bitmapSize};

    // Loop and construct our base glyph pixels.
    size_t bitmapOffset{};
    for (uint32_t &pixel : surfacePixels) { pixel = BASE_COLOR | bitmapSpan[bitmapOffset++]; }

    const std::string glyphName = "Glyph_" + std::to_string(glyphID++);
    return sdl::TextureManager::load(glyphName, tempSurface);
}