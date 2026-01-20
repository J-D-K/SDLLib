#include "TestApp.hpp"

namespace
{
    constexpr int WINDOW_WIDTH  = 1280;
    constexpr int WINDOW_HEIGHT = 720;

    constexpr SDL_Color CLEAR_COLOR = {.r = 0xEE, .g = 0xEE, .b = 0xEE, .a = 0xEE};
}

//                      ---- Construction ----

TestApp::TestApp()
    : m_sdl2{}
    , m_window{WINDOW_WIDTH, WINDOW_HEIGHT}
    , m_renderer{m_window}
    , m_input{} {};

//                      ---- Public Functions ----

int TestApp::run() noexcept
{
    for (;;)
    {
        // Update input.
        m_input.update();

        // If plus is pressed, break.
        if (m_input.button_pressed(HidNpadButton_Plus)) { return 0; }

        // Update screen.
        m_renderer.frame_begin(CLEAR_COLOR);
        m_renderer.frame_end();
    }
}
