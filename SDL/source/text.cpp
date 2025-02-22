#include "text.hpp"
#include "sdl.hpp"
#include "string.hpp"
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstring>
#include <ft2build.h>
#include <map>
#include <string>
#include <switch.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace
{
    // FreeType library and faces.
    FT_Library s_ftLib;
    FT_Face s_ftFaces[PlSharedFontType_Total];
    // Number of fonts successfully loaded.
    int32_t s_totalFonts = 0;
    // This struct holds the data needed to render each glyph.
    typedef struct
    {
            uint16_t width, height;
            int16_t advanceX, top, left;
            sdl::SharedTexture glyphTexture;
    } GlyphData;
    // Glyph cache map. Mapped according to Codepoint and font size.
    std::map<std::pair<uint32_t, int>, GlyphData> s_fontCacheMap;
    // Map of characters that are assigned to change colors.
    std::unordered_map<uint32_t, sdl::Color> s_specialCharacterMap;
    // Buffer size to use with render.
    constexpr int VA_BUFFER_SIZE = 0x1000;
    constexpr int WORD_BUFFER_SIZE = 0x100;
    // Breakpoints for line breaking.
    constexpr std::array<uint32_t, 7> BREAKPOINT_CHARS = {L' ', L'　', L'/', L'_', L'-', L'。', L'、'};
    // Masks for surfaces
    constexpr uint32_t RED_MASK = 0xFF000000;
    constexpr uint32_t GREEN_MASK = 0x00FF0000;
    constexpr uint32_t BLUE_MASK = 0x0000FF00;
    constexpr uint32_t ALPHA_MASK = 0x000000FF;
} // namespace

// Global error string
extern std::string g_sdlErrorString;

// This is so I have to type less cause I have carpal tunnel so I'm allowed to be lazy.
static inline bool freetype_error_occurred(FT_Error error)
{
    return error != 0;
}

// Resizes Freetype faces to fontSize in pixes.
static bool resize_font(int fontSize)
{
    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        FT_Error ftError = FT_Set_Pixel_Sizes(s_ftFaces[i], 0, static_cast<FT_UInt>(fontSize));
        if (freetype_error_occurred(ftError))
        {
            g_sdlErrorString = string::get_formatted_string("Error setting pixel size for font: %i.", ftError);
            return false;
        }
    }
    return true;
}

// Finds the next available point to break at and returns a pointer to it.
static size_t find_next_breakpoint(const char *string)
{
    uint32_t codepoint = 0;
    size_t stringLength = std::char_traits<char>::length(string);
    for (size_t i = 0; i < stringLength;)
    {
        // This might be kind of bad. It assumes decode_utf8 succeeded...
        i += decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&string[i]));
        if (std::find(BREAKPOINT_CHARS.begin(), BREAKPOINT_CHARS.end(), codepoint) != BREAKPOINT_CHARS.end())
        {
            return i;
        }
    }
    return stringLength;
}

static GlyphData *load_glyph(uint32_t codepoint, int fontSize)
{
    // Search cache map for glyph first.
    auto glyphCachePair = std::make_pair(codepoint, fontSize);
    if (s_fontCacheMap.find(glyphCachePair) != s_fontCacheMap.end())
    {
        return &s_fontCacheMap.at(glyphCachePair);
    }

    // Loop through the fonts loaded from the system and try to find the glyph we need.
    FT_GlyphSlot glyphSlot = nullptr;
    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        FT_UInt codepointIndex = FT_Get_Char_Index(s_ftFaces[i], codepoint);
        FT_Error ftError = FT_Load_Glyph(s_ftFaces[i], codepointIndex, FT_LOAD_RENDER);
        if (codepointIndex != 0 && !freetype_error_occurred(ftError))
        {
            glyphSlot = s_ftFaces[i]->glyph;
            break;
        }
    }

    // Just bail and return nullptr if it couldn't be found.
    if (!glyphSlot)
    {
        return nullptr;
    }

    // We're just going to assume it's gray.
    FT_Bitmap glyphBitmap = glyphSlot->bitmap;
    // Create surface to convert to texture.
    SDL_Surface *glyphSurface =
        SDL_CreateRGBSurface(0, glyphBitmap.width, glyphBitmap.rows, 32, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
    // Bail if we couldn't allocated surface.
    if (!glyphSurface)
    {
        return nullptr;
    }

    // This is where we fill in the surface.
    size_t bitmapSize = glyphBitmap.width * glyphBitmap.rows;
    uint8_t *bitmapData = glyphBitmap.buffer;
    uint32_t *surfaceData = reinterpret_cast<uint32_t *>(glyphSurface->pixels);
    for (size_t i = 0; i < bitmapSize; i++)
    {
        // Fill it in with white so we can color mod.
        *surfaceData++ = 0xFFFFFF00 | *bitmapData++;
    }

    // Texture needs a name for map.
    std::string glyphName = std::to_string(codepoint) + std::to_string(fontSize);
    // Add glyph to map.
    s_fontCacheMap[glyphCachePair] = {.width = static_cast<uint16_t>(glyphBitmap.width),
                                      .height = static_cast<uint16_t>(glyphBitmap.rows),
                                      .advanceX = static_cast<int16_t>(glyphSlot->advance.x >> 6),
                                      .top = static_cast<int16_t>(glyphSlot->bitmap_top),
                                      .left = static_cast<int16_t>(glyphSlot->bitmap_left),
                                      .glyphTexture =
                                          sdl::TextureManager::create_load_texture(glyphName, glyphSurface)};

    return &s_fontCacheMap.at(glyphCachePair);
}

// This function handles special characters that change text color.
static inline bool process_special_characters(uint32_t codepoint, sdl::Color originalColor, sdl::Color &textColor)
{
    if (s_specialCharacterMap.find(codepoint) == s_specialCharacterMap.end())
    {
        return false;
    }

    if (textColor.raw == s_specialCharacterMap[codepoint].raw)
    {
        textColor = originalColor;
    }
    else
    {
        textColor = s_specialCharacterMap[codepoint];
    }

    return true;
}

bool sdl::text::initialize(void)
{
    Result plError = plInitialize(PlServiceType_User);
    if (R_FAILED(plError))
    {
        g_sdlErrorString = string::get_formatted_string("Error initializing pl: 0x%X.", plError);
        return false;
    }

    FT_Error ftError = FT_Init_FreeType(&s_ftLib);
    if (freetype_error_occurred(ftError))
    {
        g_sdlErrorString = string::get_formatted_string("Error initializing FreeType: %i.", ftError);
        return false;
    }

    Result setError = setInitialize();
    if (R_FAILED(setError))
    {
        g_sdlErrorString = string::get_formatted_string("Error initializing set: 0x%X.", setError);
        return false;
    }

    uint64_t languageCode = 0;
    setError = setGetLanguageCode(&languageCode);
    if (R_FAILED(setError))
    {
        g_sdlErrorString = string::get_formatted_string("Error getting language code: 0x%X.", setError);
        return false;
    }

    // Load shared font from systems.
    PlFontData sharedFont[PlSharedFontType_Total];
    plError = plGetSharedFont(languageCode, sharedFont, PlSharedFontType_Total, &s_totalFonts);
    if (R_FAILED(plError))
    {
        g_sdlErrorString = string::get_formatted_string("Error loading shared fonts: 0x%X.", plError);
        return false;
    }

    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        ftError = FT_New_Memory_Face(s_ftLib,
                                     reinterpret_cast<const FT_Byte *>(sharedFont[i].address),
                                     sharedFont[i].size,
                                     0,
                                     &s_ftFaces[i]);
        if (freetype_error_occurred(ftError))
        {
            g_sdlErrorString = string::get_formatted_string("Error creating new memory face: %i.", ftError);
            return false;
        }
    }

    return true;
}

void sdl::text::exit(void)
{
    for (int32_t i = 0; i < s_totalFonts; i++)
    {
        FT_Done_Face(s_ftFaces[i]);
    }
    FT_Done_FreeType(s_ftLib);
    plExit();
}

void sdl::text::render(SDL_Texture *target,
                       int x,
                       int y,
                       int fontSize,
                       int wrapWidth,
                       sdl::Color color,
                       const char *format,
                       ...)
{
    if (!sdl::set_render_target(target))
    {
        return;
    }

    // Need to preserve original coordinates and color.
    int workingX = x;
    int workingY = y;
    sdl::Color workingColor = color;

    // Resize the font
    resize_font(fontSize);

    // va string
    char vaBuffer[VA_BUFFER_SIZE] = {0};
    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    // Loop through string and render glyphs to target.
    size_t stringLength = std::char_traits<char>::length(vaBuffer);
    for (size_t i = 0; i < stringLength;)
    {
        // This is a buffer to hold words, or characters in between breakpoints.
        char wordBuffer[WORD_BUFFER_SIZE] = {0};

        // Get the length until the next point we can break a line at, memcpy it to wordBuffer and hope really hard it doesn't write out of bounds.
        size_t wordLength = find_next_breakpoint(&vaBuffer[i]);
        std::memcpy(wordBuffer, &vaBuffer[i], wordLength);

        // If -1 was passed as wrap width, check if we need to wrap to the next line.
        if (wrapWidth != sdl::text::NO_TEXT_WRAP)
        {
            size_t wordWidth = sdl::text::get_width(fontSize, wordBuffer);
            if (workingX + wordWidth >= static_cast<unsigned int>(x + wrapWidth))
            {
                workingX = x;
                workingY += fontSize + (fontSize / 3);
            }
        }

        // Loop through word and render it.
        uint32_t codepoint = 0;
        size_t j;
        for (j = 0; j < wordLength;)
        {
            // This is bad, because it assumes decode_utf8 didn't fail.
            j += decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&wordBuffer[j]));

            // Handle special characters and line breaks.
            if (process_special_characters(codepoint, color, workingColor))
            {
                continue;
            }
            else if (codepoint == L'\n')
            {
                workingX = x;
                workingY += fontSize + (fontSize / 3);
                continue;
            }

            GlyphData *glyph = load_glyph(codepoint, fontSize);
            if (glyph && codepoint != L' ')
            {
                glyph->glyphTexture->set_color_mod(workingColor);
                glyph->glyphTexture->render(target, workingX + glyph->left, workingY + (fontSize - glyph->top));
                workingX += glyph->advanceX;
            }
            else if (glyph && codepoint == L' ')
            {
                workingX += glyph->advanceX;
            }
        }
        i += j;
    }
}

size_t sdl::text::get_width(int fontSize, const char *string)
{
    uint32_t codepoint = 0;
    size_t stringWidth = 0;
    size_t stringLength = std::char_traits<char>::length(string);
    // This is to pass to the process_special_characters function
    sdl::Color tempColor = {0x00000000};

    resize_font(fontSize);

    for (size_t i = 0; i < stringLength;)
    {
        i += decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&string[i]));

        if (process_special_characters(codepoint, tempColor, tempColor) || codepoint == L'\n')
        {
            continue;
        }

        GlyphData *glyph = load_glyph(codepoint, fontSize);
        if (glyph)
        {
            stringWidth += glyph->advanceX;
        }
    }
    return stringWidth;
}

void sdl::text::add_color_character(uint32_t codepoint, sdl::Color color)
{
    s_specialCharacterMap[codepoint] = color;
}
