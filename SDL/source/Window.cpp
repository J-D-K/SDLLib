#include "Window.hpp"

namespace
{
    /// @brief This is just a generic title since this doesn't really impact the switch?
    constexpr const char *WINDOW_TITLE = "SDL2 Switch Application";
}

//                      ---- Construction ----

sdl2::Window::Window(int windowWidth, int windowHeight)
{
    m_window = SDL_CreateWindow(WINDOW_TITLE, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!m_window) { return; }

    m_isInitialized = true;
}

sdl2::Window::~Window()
{
    if (!m_window) { return; }

    SDL_DestroyWindow(m_window);
}
