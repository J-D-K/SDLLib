#pragma once
#include "CoreComponent.hpp"
#include "Freetype.hpp"
#include "OptionalReference.hpp"
#include "ResourceManager.hpp"

#include <string_view>
#include <unordered_map>

namespace sdl2
{
    class Font : public sdl2::CoreComponent
    {
        public:
            // clang-format off
            /// @brief Struct containing cached data.
            struct GlyphData
            {
                int16_t advanceX{};
                int16_t top{};
                int16_t left{};
                sdl2::SharedTexture texture{};
            };
            // clang-format on

            /// @brief Default constructor.
            Font() = default;

            /// @brief Load an external font from file.
            /// @param fontPath Path to load the font from.
            /// @param pixelSize Size of the font in pixels.
            Font(std::string_view fontPath, int pixelSize);

            /// @brief Destructs the font.
            ~Font();

            /// @brief Renders text at the coordinates provided.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param text Text to render.
            void render_text(int x, int y, std::string_view text);

            /// @brief Renders text wrapped at the coordinates provided.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param maxWidth Maximum width of the text before it's wrapped to a new line.
            /// @param text Text to render.
            void render_text_wrapped(int x, int y, int maxWidth, std::string_view text);

            /// @brief Gets the width of the text passed.
            /// @param text Text to get the width of.
            size_t get_text_width(std::string_view text);

        protected:
            /// @brief Stores the pixel size of the font.
            int m_pixelSize{};

            /// @brief
            FT_Face m_fontFace{};

            /// @brief Buffer used for storing the font in RAM instead of reading from I/O. Gives a decent speed up.
            std::unique_ptr<FT_Byte[]> m_fontBuffer{};

            /// @brief Unordered_map used for cacheing glyph data and textures.
            std::unordered_map<uint32_t, Font::GlyphData> m_cacheMap{};

            /// @brief All instances share this instance of Freetype.
            static inline sdl2::Freetype sm_freetype{};

            /// @brief Virtual. Searches the map for the codepoint passed or loads it if needed.
            /// @param codepoint Codepoint to find or load.
            /// @return Reference to the glyph data for the code point.
            virtual OptionalReference<Font::GlyphData> find_load_glyph(uint32_t codepoint);

            /// @brief Converts the FT_Bitmap passed into an SDL_Texture/shared texture.
            /// @param codepoint Glyph codepoint.
            /// @param glyphBitmap Bitmap of the glyph.
            sdl2::SharedTexture convert_glyph_to_texture(uint32_t codepoint, const FT_Bitmap glyphBitmap);
    };
}