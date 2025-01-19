#pragma once
#include "color.hpp"
#include <SDL2/SDL.h>

namespace sdl
{
    namespace text
    {
        // This is easier to read than just putting -1 in Render.
        static constexpr int NO_TEXT_WRAP = -1;
        // Inits FreeType and loads system font.
        bool initialize(void);
        // Exits FreeType.
        void exit(void);
        /*
            Renders text at X, Y with FontSize. Returns false if an error occurs while rendering text. GetErrorString can be checked for what happened.
            WrapWidth is the width text can take up before being wrapped to a new line. Passing NO_TEXT_WRAP will ignore this.
            TextColor is the color to use. The following characters will cause this function to change color in order to allow highlighting:
                # - Blue
                * - Red
                < - Yellow
                > - Green
            They should be followed by another of the same character after the text to highlight is finished to change back to the color passed.
        */
        void render(SDL_Texture *target, int x, int y, int fontSize, int wrapWidth, sdl::Color color, const char *format, ...);
        // This returns the width of string passed.
        size_t getWidth(int fontSize, const char *string);
        // Adds a special character and corresponding color to change to when it's encountered.
        void addColorCharacter(uint32_t codepoint, sdl::Color color);
    } // namespace text
} // namespace sdl
