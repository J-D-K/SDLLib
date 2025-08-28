#include "sdl.hpp"

#include "error.hpp"

#include <SDL2/SDL_image.h>
#include <memory>
#include <string>

namespace
{
    using Window   = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
    using Renderer = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

    Renderer s_renderer = Renderer(nullptr, SDL_DestroyRenderer);
    Window s_window     = Window(nullptr, SDL_DestroyWindow);

    // SDL_image flags.
    constexpr int SDL_IMAGE_FLAGS = IMG_INIT_JPG | IMG_INIT_PNG;
} // namespace

bool sdl::initialize(const char *windowTitle, int windowWidth, int windowHeight)
{
    if (sdl::error::sdl(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))) { return false; }

    SDL_Window *tempWindow = SDL_CreateWindow(windowTitle, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (sdl::error::is_null(tempWindow)) { return false; }
    s_window.reset(tempWindow);

    SDL_Renderer *tempRenderer = SDL_CreateRenderer(s_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl::error::is_null(tempRenderer)) { return false; }
    s_renderer.reset(tempRenderer);

    int imgError = IMG_Init(SDL_IMAGE_FLAGS);
    if (imgError != SDL_IMAGE_FLAGS) { return false; }

    // This makes the icons in JKSV look nicer when I scale them, but I'm not going to fatal for it failing.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    // This isn't fatal, but I'll record it
    sdl::error::sdl(SDL_SetRenderDrawBlendMode(s_renderer.get(), SDL_BLENDMODE_BLEND));

    return true;
}

void sdl::exit()
{
    IMG_Quit();
    SDL_Quit();
}

SDL_Renderer *sdl::get_renderer() { return s_renderer.get(); }

bool sdl::frame_begin(sdl::Color color)
{
    // Set render target back to framebuffer.
    if (!sdl::set_render_target(sdl::Texture::Null) || !sdl::set_render_draw_color(color)) { return false; }
    SDL_Renderer *renderer = s_renderer.get();

    if (sdl::error::sdl(SDL_RenderClear(renderer))) { return false; }
    return true;
}

void sdl::frame_end() { SDL_RenderPresent(s_renderer.get()); }

bool sdl::set_render_target(sdl::SharedTexture &target)
{
    SDL_Renderer *renderer     = s_renderer.get();
    SDL_Texture *targetPointer = target->get();
    if (sdl::error::sdl(SDL_SetRenderTarget(renderer, targetPointer))) { return false; }
    return true;
}

bool sdl::set_render_draw_color(sdl::Color color)
{
    SDL_Renderer *renderer = s_renderer.get();
    const uint8_t red      = sdl::color::get_red(color);
    const uint8_t green    = sdl::color::get_green(color);
    const uint8_t blue     = sdl::color::get_blue(color);
    const uint8_t alpha    = sdl::color::get_alpha(color);

    if (sdl::error::sdl(SDL_SetRenderDrawColor(renderer, red, green, blue, alpha))) { return false; }
    return true;
}
bool sdl::render_line(sdl::SharedTexture &target, int xA, int yA, int xB, int yB, sdl::Color color)
{
    if (!sdl::set_render_target(target) || !sdl::set_render_draw_color(color)) { return false; }

    SDL_Renderer *renderer = s_renderer.get();
    if (sdl::error::sdl(SDL_RenderDrawLine(renderer, xA, yA, xB, yB))) { return false; };
    return true;
}

bool sdl::render_rect(sdl::SharedTexture &target, int x, int y, int width, int height, sdl::Color color)
{
    if (!sdl::set_render_target(target) || !sdl::set_render_draw_color(color)) { return false; }

    SDL_Renderer *renderer = s_renderer.get();
    const SDL_Rect rect    = {.x = x, .y = y, .w = width, .h = height};
    if (sdl::error::sdl(SDL_RenderDrawRect(renderer, &rect))) { return false; }
    return true;
}

bool sdl::render_rect_fill(sdl::SharedTexture &target, int x, int y, int width, int height, sdl::Color color)
{
    if (!sdl::set_render_target(target) || !sdl::set_render_draw_color(color)) { return false; }

    SDL_Renderer *renderer = s_renderer.get();
    const SDL_Rect rect    = {.x = x, .y = y, .w = width, .h = height};
    if (sdl::error::sdl(SDL_RenderFillRect(renderer, &rect))) { return false; }
    return true;
}
