#pragma once
#include "CoreComponent.hpp"
#include "Window.hpp"

#include <SDL2/SDL.h>
#include <memory>

namespace sdl2
{
    /// @brief Forward declaration to prevent clashes.
    class Texture;

    /// @brief Renderer wrapping class.
    class Renderer final : public sdl2::CoreComponent
    {
        public:
            /// @brief Default.
            Renderer() = default;

            /// @brief Initializes the renderer.
            /// @param window Window.
            Renderer(sdl2::Window &window);

            /// @brief Destructs renderer.
            ~Renderer();

            /// @brief Initializes the renderer.
            /// @param window Window.
            void create(sdl2::Window &window);

            /// @brief Sets the logical width and height of the renderer.
            /// @param width Width of the renderer.
            /// @param height Height of the renderer.
            bool set_logical_presentation(int width, int height);

            /// @brief Sets the render target.
            /// @param target Target to render to.
            /// @return True on success. False on failure.
            bool set_render_target(std::shared_ptr<sdl2::Texture> target);

            /// @brief Sets the render color to the color passed.
            bool set_draw_color(SDL_Color color);

            /// @brief Begins the frame and clears the target to the color passed.
            /// @param clearColor Color to clear the target with.
            bool frame_begin(SDL_Color clearColor);

            /// @brief Ends the render process and presents the target.
            void frame_end();

            /// @brief Renders a rectangle using the arguments passed.
            bool render_rectangle(int x, int y, int width, int height, SDL_Color color);

            /// @brief Renders a line using the arguments passed.
            bool render_line(int xA, int yA, int xB, int yB, SDL_Color color);

            /// @brief This allows the texture class to access and use the renderer easier.
            friend class Texture;

        private:
            /// @brief SDL Renderer.
            SDL_Renderer *m_renderer{};
    };
}