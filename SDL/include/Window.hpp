#pragma once
#include "CoreComponent.hpp"

#include <SDL2/SDL.h>

namespace sdl2
{
    /// @brief Forward declaration to prevent clashes.
    class Renderer;

    /// @brief Bare-bones encapsulation of the SDL_Window.
    class Window final : public sdl2::CoreComponent
    {
        public:
            /// @brief Creates the SDL2 window.
            /// @param windowWidth Width of the window.
            /// @param windowHeight Height of the window.
            Window(int windowWidth, int windowHeight);

            /// @brief Destroys the SDL2 Window.
            ~Window();

            /// @brief This allows the Renderer class to access the window directly.
            friend class Renderer;

        private:
            /// @brief SDL_Window.
            SDL_Window *m_window{};
    };
}