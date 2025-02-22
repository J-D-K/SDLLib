#pragma once
#include "Color.hpp"
#include "TextureManager.hpp"
#include "text.hpp"
#include <SDL2/SDL.h>

namespace sdl
{
    /// @brief Initializes SDL for video and timers.
    /// @param windowTitle Title of the window. This doesn't really matter because this is for Switch
    /// @param windowWidth Width of window.
    /// @param windowHeight Height of window.
    /// @return True on success. False on failure.
    bool initialize(const char *windowTitle, int windowWidth, int windowHeight);

    /// @brief Quits SDL
    void exit(void);

    /// @brief Returns the error string.
    /// @return Error string.
    const char *get_error_string(void);

    /// @brief Returns pointer to SDL_Renderer if needed.
    /// @return Pointer to SDL_Renderer.
    SDL_Renderer *get_renderer(void);

    /// @brief Begins a frame and clears it to color.
    /// @param color Color to clear the frame to.
    /// @return True on success. False on failure.
    bool frame_begin(sdl::Color color);

    /// @brief Ends a frame and presents it to screen.
    void frame_end(void);

    /// @brief Sets the current render target texture.
    /// @param target Target texture.
    /// @return True on success. False on failure.
    bool set_render_target(SDL_Texture *target);

    /// @brief Sets the color the renderer will use to render or draw with.
    /// @param color Color to draw with.
    /// @return True on success. False on failure.
    bool set_render_draw_color(sdl::Color color);

    /// @brief Renders a line to target.
    /// @param target Render target.
    /// @param xA First X coordinate.
    /// @param yA First Y coordinate.
    /// @param xB Second X coordinate.
    /// @param yB Second Y coordinate.
    /// @param color Color to render line with.
    /// @return True on success. False on failure.
    bool render_line(SDL_Texture *target, int xA, int yA, int xB, int yB, sdl::Color color);

    /// @brief Renders a filled rectangle.
    /// @param target Target to render to.
    /// @param x X position.
    /// @param y Y position.
    /// @param width Width.
    /// @param height Height.
    /// @param color Render color.
    /// @return True on success. False on failure.
    bool render_rect_fill(SDL_Texture *target, int x, int y, int width, int height, sdl::Color color);

    /// @brief Inline function I wrote so I wouldn't have to type so much.
    /// @param sdlError Return value from SDL function.
    /// @return If it's an error.
    inline bool error_occurred(int SDLError)
    {
        return SDLError != 0;
    }
} // namespace sdl
