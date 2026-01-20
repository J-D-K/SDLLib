#pragma once
#include "CoreComponent.hpp"
#include "Window.hpp"

#include <SDL2/SDL.h>

namespace sdl2
{
    class Renderer final : public sdl2::CoreComponent
    {
        public:
            /// @brief Initializes the renderer.
            /// @param window Window.
            Renderer(sdl2::Window &window);

            /// @brief Destructs renderer.
            ~Renderer();

            /// @brief Begins the frame and clears the target to the color passed.
            /// @param clearColor Color to clear the target with.
            bool frame_begin(SDL_Color clearColor);

            /// @brief Ends the render process and presents the target.
            void frame_end();

        private:
            /// @brief SDL Renderer.
            SDL_Renderer *m_renderer{};
    };
}