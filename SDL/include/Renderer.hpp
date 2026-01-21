#pragma once
#include "CoreComponent.hpp"
#include "Window.hpp"

#include <SDL2/SDL.h>

namespace sdl2
{
    /// @brief Forward declaration to prevent clashes.
    class Texture;

    /// @brief Renderer wrapping class.
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

            /// @brief This allows the texture class to access and use the renderer easier.
            friend class Texture;

        private:
            /// @brief SDL Renderer.
            SDL_Renderer *m_renderer{};
    };
}