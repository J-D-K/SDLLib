#include "sdl.hpp"
#include "string.hpp"
#include <SDL2/SDL_image.h>
#include <string>

namespace
{
    // Window and renderer.
    SDL_Window   *s_window = NULL;
    SDL_Renderer *s_renderer = NULL;
    // These are the flags for SDL_image
    constexpr int SDL_IMAGE_FLAGS = IMG_INIT_JPG | IMG_INIT_PNG;
} // namespace

std::string g_sdlErrorString = "No errors encountered.";

bool sdl::initialize(const char *windowTitle, int windowWidth, int windowHeight)
{
    int sdlError = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error initializing SDL: %s.", SDL_GetError());
        return false;
    }

    s_window = SDL_CreateWindow(windowTitle, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!s_window)
    {
        g_sdlErrorString = string::getFormattedString("Error creating SDL_Window: %s.", SDL_GetError());
        return false;
    }

    s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!s_renderer)
    {
        g_sdlErrorString = string::getFormattedString("Error creating SDL_Renderer: %s", SDL_GetError());
        return false;
    }

    sdlError = IMG_Init(SDL_IMAGE_FLAGS);
    if (sdlError != SDL_IMAGE_FLAGS)
    {
        g_sdlErrorString = string::getFormattedString("Error initializing SDL_image: %s.", IMG_GetError());
        return false;
    }

    // This makes the icons in JKSV look nicer when I scale them, but I'm not going to fatal for it failing.
    SDL_bool hintError = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    if (!hintError)
    {
        g_sdlErrorString = string::getFormattedString("Error setting texture scale quality: %s.", SDL_GetError());
    }

    sdlError = SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error setting SDL blend mode: %s", SDL_GetError());
    }

    return true;
}

void sdl::exit(void)
{
    if (s_renderer)
    {
        SDL_DestroyRenderer(s_renderer);
    }

    if (s_window)
    {
        SDL_DestroyWindow(s_window);
    }
    IMG_Quit();
    SDL_Quit();
}

const char *sdl::getErrorString(void)
{
    return g_sdlErrorString.c_str();
}

SDL_Renderer *sdl::getRenderer(void)
{
    return s_renderer;
}

bool sdl::frameBegin(sdl::Color color)
{
    // Set render target back to framebuffer.
    if (!sdl::setRenderTarget(NULL) || !sdl::setRenderDrawColor(color))
    {
        return false;
    }

    int sdlError = SDL_RenderClear(s_renderer);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error clearing renderer: %s.", SDL_GetError());
        return false;
    }
    return true;
}

void sdl::frameEnd(void)
{
    SDL_RenderPresent(s_renderer);
}

bool sdl::setRenderTarget(SDL_Texture *target)
{
    int sdlError = SDL_SetRenderTarget(s_renderer, target);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error setting render target: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool sdl::setRenderDrawColor(sdl::Color color)
{
    int sdlError = SDL_SetRenderDrawColor(s_renderer, color.rgba[3], color.rgba[2], color.rgba[1], color.rgba[0]);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error setting renderer draw color: %s.", SDL_GetError());
        return false;
    }
    return true;
}
bool sdl::renderLine(SDL_Texture *target, int xA, int yA, int xB, int yB, sdl::Color color)
{
    if (!sdl::setRenderTarget(target) || !sdl::setRenderDrawColor(color))
    {
        return false;
    }

    int sdlError = SDL_RenderDrawLine(s_renderer, xA, yA, xB, yB);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error rendering line: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool sdl::renderRectFill(SDL_Texture *target, int x, int y, int width, int height, sdl::Color color)
{
    if (!sdl::setRenderTarget(target) || !sdl::setRenderDrawColor(color))
    {
        return false;
    }

    SDL_Rect rect = {.x = x, .y = y, .w = width, .h = height};
    int      sdlError = SDL_RenderFillRect(s_renderer, &rect);
    if (sdl::errorOccurred(sdlError))
    {
        g_sdlErrorString = string::getFormattedString("Error rendering filled rectangle: %s.", SDL_GetError());
        return false;
    }
    return true;
}
