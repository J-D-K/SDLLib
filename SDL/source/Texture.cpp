#include "Texture.hpp"

#include <SDL2/SDL_image.h>

#define RETURN_ON_INVALID_RENDERER(renderer)                                                                                   \
    if (!renderer) { return; }

#define RETURN_ON_INVALID_TEXTURE(renderer, texture)                                                                           \
    if (!renderer || !texture) { return false; }

//                      ---- Construction ----

sdl2::Texture::Texture(std::string_view filePath)
{
    RETURN_ON_INVALID_RENDERER(sm_renderer);

    // Load the texture.
    m_texture = IMG_LoadTexture(sm_renderer, filePath.data());

    // Query and get its width and height.
    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height);

    m_isInitialized = true;
}

sdl2::Texture::Texture(sdl2::Surface &surface)
    : m_width(surface.get()->w)
    , m_height(surface.get()->h)
{
    RETURN_ON_INVALID_RENDERER(sm_renderer);

    m_texture       = SDL_CreateTextureFromSurface(sm_renderer, surface.get());
    m_isInitialized = true;
}

sdl2::Texture::Texture(int width, int height, SDL_TextureAccess textureAccess)
    : m_width(width)
    , m_height(height)
{
    RETURN_ON_INVALID_RENDERER(sm_renderer);

    m_texture       = SDL_CreateTexture(sm_renderer, SDL_PIXELFORMAT_ARGB8888, textureAccess, width, height);
    m_isInitialized = true;
}

sdl2::Texture::~Texture()
{
    if (!m_texture) { return; }

    SDL_DestroyTexture(m_texture);
}

//                      ---- Public Functions ----

int sdl2::Texture::get_width() const noexcept { return m_width; }

int sdl2::Texture::get_height() const noexcept { return m_height; }

bool sdl2::Texture::render(int x, int y)
{
    // Bail if these are set.
    RETURN_ON_INVALID_TEXTURE(sm_renderer, m_texture);

    // Render rect coordinates.
    const SDL_Rect sourceRect = {.x = 0, .y = 0, .w = m_width, .h = m_height};
    const SDL_Rect destRect   = {.x = x, .y = y, .w = m_width, .h = m_height};

    // Return success or not.
    return SDL_RenderCopy(sm_renderer, m_texture, &sourceRect, &destRect) == 0;
}

bool sdl2::Texture::render_part(int x, int y, int sourceX, int sourceY, int sourceWidth, int sourceHeight)
{
    RETURN_ON_INVALID_TEXTURE(sm_renderer, m_texture);

    const SDL_Rect sourceRect = {.x = sourceX, .y = sourceY, .w = sourceWidth, .h = sourceHeight};
    const SDL_Rect destRect   = {.x = x, .y = y, .w = sourceWidth, .h = sourceHeight};

    return SDL_RenderCopy(sm_renderer, m_texture, &sourceRect, &destRect) == 0;
}

bool sdl2::Texture::render_stretched(int x, int y, int width, int height)
{
    RETURN_ON_INVALID_TEXTURE(sm_renderer, m_texture);

    const SDL_Rect sourceRect = {.x = 0, .y = 0, .w = m_width, .h = m_height};
    const SDL_Rect destRect   = {.x = x, .y = y, .w = width, .h = m_height};

    return SDL_RenderCopy(sm_renderer, m_texture, &sourceRect, &destRect) == 0;
}

bool sdl2::Texture::render_part_stretched(int x,
                                          int y,
                                          int width,
                                          int height,
                                          int sourceX,
                                          int sourceY,
                                          int sourceWidth,
                                          int sourceHeight)
{
    RETURN_ON_INVALID_TEXTURE(sm_renderer, m_texture);

    const SDL_Rect sourceRect = {.x = sourceX, .y = sourceY, .w = sourceWidth, .h = sourceHeight};
    const SDL_Rect destRect   = {.x = x, .y = y, .w = width, .h = height};

    return SDL_RenderCopy(sm_renderer, m_texture, &sourceRect, &destRect) == 0;
}

void sdl2::Texture::initialize(const sdl2::Renderer &renderer) { sm_renderer = renderer.m_renderer; }
