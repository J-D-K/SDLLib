#include "Texture.hpp"

#include "error.hpp"
#include "sdl.hpp"

#include <SDL2/SDL_image.h>
#include <cstdio>

sdl::Texture::Texture(const char *imagePath)
{
    m_texture = IMG_LoadTexture(sdl::get_renderer(), imagePath);
    if (sdl::error::is_null(m_texture)) { return; }
    if (sdl::error::sdl(SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height))) { return; }
    Texture::enable_blending();
}

sdl::Texture::Texture(sdl::Surface &surface)
{
    m_width  = surface->w;
    m_height = surface->h;

    m_texture = SDL_CreateTextureFromSurface(sdl::get_renderer(), surface.get());
    if (sdl::error::is_null(m_texture)) { return; }
    Texture::enable_blending();
}

sdl::Texture::Texture(const void *data, size_t dataSize)
{
    SDL_RWops *imageReadOps = SDL_RWFromConstMem(data, dataSize);
    m_texture               = IMG_LoadTexture_RW(sdl::get_renderer(), imageReadOps, 1);
    if (sdl::error::is_null(m_texture)) { return; }
    if (sdl::error::sdl(SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height))) { return; }
    Texture::enable_blending();
}

sdl::Texture::Texture(int width, int height, int accessFlags)
{
    m_width  = width;
    m_height = height;

    m_texture = SDL_CreateTexture(sdl::get_renderer(), SDL_PIXELFORMAT_RGBA8888, accessFlags, width, height);
    if (sdl::error::is_null(m_texture)) { return; }
    Texture::enable_blending();
}

sdl::Texture::Texture(Texture &&texture) { *this = std::move(texture); }

sdl::Texture &sdl::Texture::operator=(Texture &&texture)
{
    m_texture     = texture.m_texture;
    m_width       = texture.m_width;
    m_height      = texture.m_height;
    m_accessFlags = texture.m_accessFlags;

    texture.m_texture     = nullptr;
    texture.m_width       = 0;
    texture.m_height      = 0;
    texture.m_accessFlags = 0;

    return *this;
}

sdl::Texture::~Texture()
{
    if (m_texture) { SDL_DestroyTexture(m_texture); }
}

SDL_Texture *sdl::Texture::get() { return m_texture; }

bool sdl::Texture::render(sdl::SharedTexture &target, int x, int y)
{
    if (!sdl::set_render_target(target)) { return false; }

    SDL_Rect source      = {.x = 0, .y = 0, .w = m_width, .h = m_height};
    SDL_Rect destination = {.x = x, .y = y, .w = m_width, .h = m_height};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::render_stretched(sdl::SharedTexture &target, int x, int y, int width, int height)
{
    if (!sdl::set_render_target(target)) { return false; }

    SDL_Rect source      = {.x = 0, .y = 0, .w = m_width, .h = m_height};
    SDL_Rect destination = {.x = x, .y = y, .w = width, .h = height};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::render_part(sdl::SharedTexture &target,
                               int x,
                               int y,
                               int sourceX,
                               int sourceY,
                               int sourceWidth,
                               int sourceHeight)
{
    if (!sdl::set_render_target(target)) { return false; }

    SDL_Rect source      = {.x = sourceX, .y = sourceY, .w = sourceWidth, .h = sourceHeight};
    SDL_Rect destination = {.x = x, .y = y, .w = sourceWidth, .h = sourceHeight};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::render_part_stretched(sdl::SharedTexture &target,
                                         int sourceX,
                                         int sourceY,
                                         int sourceWidth,
                                         int sourceHeight,
                                         int destinationX,
                                         int destinationY,
                                         int destinationWidth,
                                         int destinationHeight)
{
    if (!sdl::set_render_target(target)) { return false; }

    SDL_Rect source      = {.x = sourceX, .y = sourceY, .w = sourceWidth, .h = sourceHeight};
    SDL_Rect destination = {.x = destinationX, .y = destinationY, .w = destinationWidth, .h = destinationHeight};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::clear(sdl::Color color)
{
    SDL_Renderer *renderer = sdl::get_renderer();

    // This needs to bypass.
    if (sdl::error::sdl(SDL_SetRenderTarget(renderer, m_texture)) || !sdl::set_render_draw_color(color)) { return false; }
    if (sdl::error::sdl(SDL_RenderClear(renderer))) { return false; }
    return true;
}

bool sdl::Texture::set_color_mod(sdl::Color color)
{
    const uint8_t red   = sdl::color::get_red(color);
    const uint8_t green = sdl::color::get_green(color);
    const uint8_t blue  = sdl::color::get_blue(color);

    if (sdl::error::sdl(SDL_SetTextureColorMod(m_texture, red, green, blue))) { return false; }
    return true;
}

void sdl::Texture::enable_blending() { sdl::error::sdl(SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND)); }

bool sdl::Texture::render_copy(const SDL_Rect *source, const SDL_Rect *destination)
{
    SDL_Renderer *renderer = sdl::get_renderer();
    if (sdl::error::sdl(SDL_RenderCopy(renderer, m_texture, source, destination))) { return false; }
    return true;
}
