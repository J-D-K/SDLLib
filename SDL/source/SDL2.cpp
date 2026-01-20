#include "SDL2.hpp"

#include <SDL2/SDL.h>
#include <cstdint>

namespace
{
    /// @brief Flags used to initialize SDL2.
    constexpr uint32_t SDL_INIT_FLAGS = SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO;
}

//                      ---- Construction ----

sdl2::SDL2::SDL2()
{
    const int sdlError = SDL_Init(SDL_INIT_FLAGS);
    if (sdlError != 0) { return; }

    m_isInitialized = true;
}

sdl2::SDL2::~SDL2() { SDL_Quit(); }
