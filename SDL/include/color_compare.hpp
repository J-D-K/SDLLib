#pragma once
#include <SDL2/SDL.h>

static inline bool operator==(SDL_Color colorA, SDL_Color colorB) noexcept
{
    return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a;
}