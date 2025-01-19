#pragma once
#include "color.hpp"
#include "text.hpp"
#include "textureManager.hpp"
#include <SDL2/SDL.h>

namespace sdl
{
    // Initializes SDL and SDL_image.
    bool initialize(const char *windowTitle, int windowWidth, int windowHeight);
    // Exits SDL
    void exit(void);
    // Returns internal error string.
    const char *getErrorString(void);
    // Returns renderer if needed.
    SDL_Renderer *getRenderer(void);
    // Begins and frame and clears the renderer to ClearColor
    bool frameBegin(sdl::Color color);
    // Ends and frame and presents it to screen.
    void frameEnd(void);
    // Sets the render target.
    bool setRenderTarget(SDL_Texture *target);
    // Sets the render draw color.
    bool setRenderDrawColor(sdl::Color color);
    // I don't feel like these need a separate file.
    bool renderLine(SDL_Texture *target, int xA, int yA, int xB, int yB, sdl::Color color);
    bool renderRectFill(SDL_Texture *target, int x, int y, int width, int height, sdl::Color color);

    // I got tired of type SDLError != 0.z
    inline bool errorOccurred(int SDLError)
    {
        return SDLError != 0;
    }
} // namespace sdl
