#include "Renderer.hpp"

#include <cstdint>

namespace
{
    /// @brief Flags used for creating the renderer.
    constexpr uint32_t SDL_RENDER_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
}

//                      ---- Construction ----

sdl2::Renderer::Renderer(sdl2::Window &window)
{
    m_renderer = SDL_CreateRenderer(window.m_window, -1, SDL_RENDER_FLAGS);
    if (!m_renderer) { return; }

    m_isInitialized = true;
}

sdl2::Renderer::~Renderer()
{
    if (!m_renderer) { return; }

    SDL_DestroyRenderer(m_renderer);
}

//                      ---- Public Functions ----

bool sdl2::Renderer::set_logical_presentation(int width, int height)
{
    return SDL_RenderSetLogicalSize(m_renderer, width, height) == 0;
}

bool sdl2::Renderer::frame_begin(SDL_Color clearColor)
{
    // Start by clearing.
    // Set the draw color.
    int sdlError = SDL_SetRenderDrawColor(m_renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    if (sdlError != 0) { return false; }

    // Clear the renderer.
    sdlError = SDL_RenderClear(m_renderer);
    if (sdlError != 0) { return false; }

    return true;
}

void sdl2::Renderer::frame_end() { SDL_RenderPresent(m_renderer); }