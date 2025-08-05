#include "Surface.hpp"

#include <SDL2/SDL_image.h>

namespace
{
    constexpr int BIT_DEPTH = 32;
    constexpr int BIT_PITCH = 32;

    constexpr uint32_t MASK_RED   = 0xFF000000;
    constexpr uint32_t MASK_GREEN = 0x00FF0000;
    constexpr uint32_t MASK_BLUE  = 0x0000FF00;
    constexpr uint32_t MASK_ALPHA = 0x000000FF;
}

static sdl::Surface from_pointer(SDL_Surface *surface);

sdl::Surface sdl::surface::create_rgb(int width, int height)
{
    SDL_Surface *surface{};
    surface = SDL_CreateRGBSurface(0, width, height, BIT_DEPTH, MASK_RED, MASK_GREEN, MASK_BLUE, MASK_ALPHA);
    return from_pointer(surface);
}

sdl::Surface sdl::surface::from_file(std::string_view filePath)
{
    SDL_Surface *surface{};
    surface = IMG_Load(filePath.data());
    return from_pointer(surface);
}

sdl::Surface sdl::surface::from_memory(const void *data, size_t dataSize)
{
    SDL_Surface *surface{};
    SDL_RWops *ops = SDL_RWFromConstMem(data, dataSize);

    surface = IMG_Load_RW(ops, 1);
    return from_pointer(surface);
}

static sdl::Surface from_pointer(SDL_Surface *surface) { return sdl::Surface(surface, SDL_FreeSurface); }