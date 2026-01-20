#include "sdl.hpp"

namespace
{
    // Width and height of window.
    constexpr int WINDOW_WIDTH  = 1280;
    constexpr int WINDOW_HEIGHT = 720;

    // Clear color.
    constexpr SDL_Color CLEAR_COLOR = {.r = 0x2D, .g = 0x2D, .b = 0x2D, .a = 0xFF};

}

int main(void)
{
    // Init SDL.
    sdl2::SDL2 sdl{};
    if (!sdl.is_initialized()) { return -1; }

    // Window.
    sdl2::Window window{WINDOW_WIDTH, WINDOW_HEIGHT};
    if (!window.is_initialized()) { return -2; }

    // Renderer.
    sdl2::Renderer renderer{window};
    if (!renderer.is_initialized()) { return -3; }

    // Input.
    sdl2::Input input{};

    while (true)
    {
        // Update input.
        input.update();

        if (input.button_pressed(HidNpadButton_Plus)) { break; }

        renderer.frame_begin(CLEAR_COLOR);
        renderer.frame_end();
    }

    return 0;
}