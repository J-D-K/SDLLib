#pragma once
#include "CoreComponent.hpp"
#include "Freetype.hpp"
#include "OptionalReference.hpp"
#include "ResourceManager.hpp"

#include <SDL2/SDL.h>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

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
            virtual ~Font();

            /// @brief Renders text at the coordinates provided.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param color Color to render text with.
            /// @param text Text to render.
            void render_text(int x, int y, SDL_Color color, std::string_view text);

            /// @brief Renders text wrapped at the coordinates provided.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param maxWidth Maximum width of the text before it's wrapped to a new line.
            /// @param text Text to render.
            void render_text_wrapped(int x, int y, SDL_Color color, int maxWidth, std::string_view text);

            /// @brief Gets the width of the text passed.
            /// @param text Text to get the width of.
            int get_text_width(std::string_view text);

            /// @brief Adds a codepoint to the list of characters to break at for rendering wrapped text.
            /// @param codepoint Codepoint to enable line breaking at.
            static void add_break_point(uint32_t codepoint);

            /// @brief Adds a list of breakpoints to the breakpoint list.
            /// @param pointList List of breakpoints to add.
            static void add_break_points(std::initializer_list<uint32_t> pointList);

            /// @brief Adds a span of codepoints to the line breaking list.
            /// @param pointSpan Span of codepoints to believe.
            static void add_break_points(std::span<uint32_t> pointSpan);

            /// @brief Adds a codepoint to the array of characters that change rendering color.
            /// @param codepoint Codepoint to enable color changing for.
            /// @param color Color to change to.
            static void add_color_point(uint32_t codepoint, SDL_Color color);

            /// @brief Adds a list of color changing codepoints.
            /// @param pointList Initializer list of color changing points paired with their color.
            static void add_color_points(std::initializer_list<std::pair<uint32_t, SDL_Color>> pointList);

            /// @brief Adds a list of color changing codepoints.
            /// @param pointSpan Span containing codepoints paired with their color.
            static void add_color_points(std::span<std::pair<uint32_t, SDL_Color>> pointSpan);

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

        private:
            /// @brief Vector of breakpoints for wrapping.
            static inline std::vector<uint32_t> sm_breakPoints{};

            /// @brief Vector of color changing codepoints.
            static inline std::vector<std::pair<uint32_t, SDL_Color>> sm_colorPoints{};

            /// @brief Locates the next breakpoint in the string starting from i.
            size_t find_next_breakpoint(std::string_view string);

            /// @brief Returns whether or not the codepoint passed is a breakpoint.
            /// @param codepoint Codepoint to check.
            bool is_breakpoint(uint32_t codepoint) const noexcept;

            /// @brief Returns whether or not the codepoint passed is a color changing codepoint.
            /// @param codepoint Codepoint to check.
            bool is_color_point(uint32_t codepoint) const noexcept;

            /// @brief Returns the color of the codepoint passed.
            SDL_Color get_point_color(uint32_t codepoint) const noexcept;
    };
}