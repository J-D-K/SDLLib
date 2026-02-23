#include "Window.hpp"

//                      ---- Construction ----

sdl2::Window::Window(int windowWidth, int windowHeight) { Window::create(windowWidth, windowHeight); }

sdl2::Window::~Window()
{
    if (!m_window) { return; }

    SDL_DestroyWindow(m_window);
}

//                      ---- Public functions ----

void sdl2::Window::create(int windowWidth, int windowHeight)
{
    // This isn't actually visible anywhere on Switch so...
    static constexpr const char *WINDOW_TITLE = "SDL2_Switch";

    m_window = SDL_CreateWindow(WINDOW_TITLE, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!m_window) { return; }

    m_isInitialized = true;
}