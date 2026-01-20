#include "TestApp.hpp"
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
    TestApp testApp{};
    return testApp.run();
}