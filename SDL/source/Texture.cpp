#include "Texture.hpp"
#include "sdl.hpp"
#include "string.hpp"
#include <SDL2/SDL_image.h>
#include <cstdio>

namespace
{
    const char *ERROR_QUERYING_TEXTURE = "Error querying texture: %s";
} // namespace

// Global error string
extern std::string g_sdlErrorString;

sdl::Texture::Texture(const char *imagePath)
{
    m_texture = IMG_LoadTexture(sdl::get_renderer(), imagePath);
    if (!m_texture)
    {
        g_sdlErrorString = string::get_formatted_string("Error loading texture from file: %s.", IMG_GetError());
        return;
    }

    int sdlError = SDL_QueryTexture(m_texture, NULL, NULL, &m_width, &m_height);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string(ERROR_QUERYING_TEXTURE, IMG_GetError());
        return;
    }
    Texture::enable_blending();
}

sdl::Texture::Texture(SDL_Surface *surface, bool freeSurface)
{
    m_width = surface->w;
    m_height = surface->h;

    m_texture = SDL_CreateTextureFromSurface(sdl::get_renderer(), surface);
    if (!m_texture)
    {
        g_sdlErrorString = string::get_formatted_string("Error creating texture from surface: %s.", SDL_GetError());
        return;
    }

    if (freeSurface)
    {
        SDL_FreeSurface(surface);
    }
    Texture::enable_blending();
}

sdl::Texture::Texture(const void *data, size_t dataSize)
{
    SDL_RWops *imageReadOps = SDL_RWFromConstMem(data, dataSize);
    // I'm assuming this reads the data header and determines what the format is?
    m_texture = IMG_LoadTexture_RW(sdl::get_renderer(), imageReadOps, 1);

    if (!m_texture)
    {
        g_sdlErrorString = string::get_formatted_string("Error loading texture from memory: %s.", IMG_GetError());
        return;
    }

    int sdlError = SDL_QueryTexture(m_texture, NULL, NULL, &m_width, &m_height);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string(ERROR_QUERYING_TEXTURE, SDL_GetError());
        return;
    }

    Texture::enable_blending();
}

sdl::Texture::Texture(int width, int height, int accessFlags)
{
    m_width = width;
    m_height = height;

    m_texture = SDL_CreateTexture(sdl::get_renderer(), SDL_PIXELFORMAT_RGBA8888, accessFlags, width, height);
    if (!m_texture)
    {
        g_sdlErrorString = string::get_formatted_string("Error creating texture: %s.", SDL_GetError());
        return;
    }
    Texture::enable_blending();
}

sdl::Texture::~Texture()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
    }
}

SDL_Texture *sdl::Texture::get(void)
{
    return m_texture;
}

bool sdl::Texture::render(SDL_Texture *target, int x, int y)
{
    if (!sdl::set_render_target(target))
    {
        return false;
    }

    SDL_Rect source = {.x = 0, .y = 0, .w = m_width, .h = m_height};
    SDL_Rect destination = {.x = x, .y = y, .w = m_width, .h = m_height};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::render_stretched(SDL_Texture *target, int x, int y, int width, int height)
{
    if (!sdl::set_render_target(target))
    {
        return false;
    }

    SDL_Rect source = {.x = 0, .y = 0, .w = m_width, .h = m_height};
    SDL_Rect destination = {.x = x, .y = y, .w = width, .h = height};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::render_part(SDL_Texture *target,
                               int x,
                               int y,
                               int sourceX,
                               int sourceY,
                               int sourceWidth,
                               int sourceHeight)
{
    if (!sdl::set_render_target(target))
    {
        return false;
    }

    SDL_Rect source = {.x = sourceX, .y = sourceY, .w = sourceWidth, .h = sourceHeight};
    SDL_Rect destination = {.x = x, .y = y, .w = sourceWidth, .h = sourceHeight};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::render_part_stretched(SDL_Texture *target,
                                         int sourceX,
                                         int sourceY,
                                         int sourceWidth,
                                         int sourceHeight,
                                         int destinationX,
                                         int destinationY,
                                         int destinationWidth,
                                         int destinationHeight)
{
    if (!sdl::set_render_target(target))
    {
        return false;
    }

    SDL_Rect source = {.x = sourceX, .y = sourceY, .w = sourceWidth, .h = sourceHeight};
    SDL_Rect destination = {.x = destinationX, .y = destinationY, .w = destinationWidth, .h = destinationHeight};
    return Texture::render_copy(&source, &destination);
}

bool sdl::Texture::clear(sdl::Color color)
{
    if (!sdl::set_render_target(m_texture) || !sdl::set_render_draw_color(color))
    {
        return false;
    }

    int sdlError = SDL_RenderClear(sdl::get_renderer());
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error clearing texture: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool sdl::Texture::resize(int width, int height)
{
    // Destroy current texture.
    SDL_DestroyTexture(m_texture);

    // Create a new one in its place.
    m_texture = SDL_CreateTexture(sdl::get_renderer(), SDL_PIXELFORMAT_RGBA8888, m_accessFlags, width, height);
    if (!m_texture)
    {
        g_sdlErrorString = string::get_formatted_string("Error resizing texture: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool sdl::Texture::set_color_mod(sdl::Color color)
{
    int sdlError = SDL_SetTextureColorMod(m_texture, color.rgba[3], color.rgba[2], color.rgba[1]);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error setting color mod: %s.", SDL_GetError());
        return false;
    }
    return true;
}

void sdl::Texture::enable_blending(void)
{
    int sdlError = SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error setting blend mode for texture: %s.", SDL_GetError());
    }
}

bool sdl::Texture::render_copy(const SDL_Rect *source, const SDL_Rect *destination)
{
    int sdlError = SDL_RenderCopy(sdl::get_renderer(), m_texture, source, destination);
    if (sdl::error_occurred(sdlError))
    {
        g_sdlErrorString = string::get_formatted_string("Error rendering texture: %s.", SDL_GetError());
        return false;
    }
    return true;
}
