#pragma once
#include "Color.hpp"
#include <SDL2/SDL.h>

namespace sdl
{
    /// @brief Namespace for text related functions.
    namespace text
    {
        /// @brief Constant value that tells text::render to not wrap lines.
        static constexpr int NO_TEXT_WRAP = -1;

        /// @brief Initializes FreeType and the Switch's shared system font.
        /// @return True on success. False on failure.
        bool initialize(void);

        /// @brief Exits FreeType.
        void exit(void);

        /// @brief Renders text.
        /// @param target Render target.
        /// @param x X coordinate.
        /// @param y Y coordinate.
        /// @param fontSize Font size in pixels to use.
        /// @param wrapWidth How wide text can be before being wrapped to a new line. Pass -1 or sdl::text::NO_TEXT_WRAP for no wrapping.
        /// @param color The main color to use to render text. Note: Any characters set with addColorCharacter will change this when found.
        /// @param format Format of the string to render.
        /// @param Arguments Va style (printf) style arguments.
        void render(SDL_Texture *target, int x, int y, int fontSize, int wrapWidth, sdl::Color color, const char *format, ...);

        /// @brief Gets the width of the text in pixels.
        /// @param fontSize Size of the font in pixels.
        /// @param string  String to get the width of.
        /// @return Width of string in pixels.
        size_t getWidth(int fontSize, const char *string);

        /// @brief Adds a special character that changes the rendering color of text.
        /// @param codepoint Character to use.
        /// @param color Color it changes text to in between two occurrences of the character to.
        void addColorCharacter(uint32_t codepoint, sdl::Color color);
    } // namespace text
} // namespace sdl
