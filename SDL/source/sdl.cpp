#include "sdl.hpp"
#include "string.hpp"
#include <SDL2/SDL_image.h>
#include <string>

namespace
{
    // Window and renderer.
    SDL_Window *s_window = NULL;
    SDL_Renderer *s_renderer = NULL;
    // These are the flags for SDL_image
    constexpr int SDL_IMAGE_FLAGS = IMG_INIT_JPG | IMG_INIT_PNG;
} // namespace

std::string g_sdlErrorString = "No errors encountered.";

bool sdl::initialize(const char *windowTitle, int windowWidth, int windowHeight)
{
    int sdlError = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error initializing SDL: %s.", SDL_GetError());
        return false;
    }

    s_window = SDL_CreateWindow(windowTitle, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!s_window)
    {
        g_sdlErrorString = string::get_formatted_string("Error creating SDL_Window: %s.", SDL_GetError());
        return false;
    }

    s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!s_renderer)
    {
        g_sdlErrorString = string::get_formatted_string("Error creating SDL_Renderer: %s", SDL_GetError());
        return false;
    }

    sdlError = IMG_Init(SDL_IMAGE_FLAGS);
    if (sdlError != SDL_IMAGE_FLAGS)
    {
        g_sdlErrorString = string::get_formatted_string("Error initializing SDL_image: %s.", IMG_GetError());
        return false;
    }

    // This makes the icons in JKSV look nicer when I scale them, but I'm not going to fatal for it failing.
    SDL_bool hintError = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    if (!hintError)
    {
        g_sdlErrorString = string::get_formatted_string("Error setting texture scale quality: %s.", SDL_GetError());
    }

    sdlError = SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error setting SDL blend mode: %s", SDL_GetError());
    }

    return true;
}

void sdl::exit()
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

const char *sdl::get_error_string()
{
    return g_sdlErrorString.c_str();
}

SDL_Renderer *sdl::get_renderer()
{
    return s_renderer;
}

bool sdl::frame_begin(sdl::Color color)
{
    // Set render target back to framebuffer.
    if (!sdl::set_render_target(NULL) || !sdl::set_render_draw_color(color))
    {
        return false;
    }

    int sdlError = SDL_RenderClear(s_renderer);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error clearing renderer: %s.", SDL_GetError());
        return false;
    }
    return true;
}

void sdl::frame_end()
{
    SDL_RenderPresent(s_renderer);
}

bool sdl::set_render_target(SDL_Texture *target)
{
    int sdlError = SDL_SetRenderTarget(s_renderer, target);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error setting render target: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool sdl::set_render_draw_color(sdl::Color color)
{
    int sdlError = SDL_SetRenderDrawColor(s_renderer, color.rgba[3], color.rgba[2], color.rgba[1], color.rgba[0]);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error setting renderer draw color: %s.", SDL_GetError());
        return false;
    }
    return true;
}
bool sdl::render_line(SDL_Texture *target, int xA, int yA, int xB, int yB, sdl::Color color)
{
    if (!sdl::set_render_target(target) || !sdl::set_render_draw_color(color))
    {
        return false;
    }

    int sdlError = SDL_RenderDrawLine(s_renderer, xA, yA, xB, yB);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error rendering line: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool sdl::render_rect_fill(SDL_Texture *target, int x, int y, int width, int height, sdl::Color color)
{
    if (!sdl::set_render_target(target) || !sdl::set_render_draw_color(color))
    {
        return false;
    }

    SDL_Rect rect = {.x = x, .y = y, .w = width, .h = height};
    int sdlError = SDL_RenderFillRect(s_renderer, &rect);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error rendering filled rectangle: %s.", SDL_GetError());
        return false;
    }
    return true;
}
