#include "TestApp.hpp"

namespace
{
    constexpr int WINDOW_WIDTH  = 1280;
    constexpr int WINDOW_HEIGHT = 720;

    constexpr SDL_Color CLEAR_COLOR = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF};

    constexpr std::string_view FONT_PATH = "romfs:/assets/MainFont.ttf";
    constexpr int FONT_SIZE              = 24;
}

//                      ---- Construction ----

TestApp::TestApp()
    : m_sdl2{}
    , m_window{WINDOW_WIDTH, WINDOW_HEIGHT}
    , m_renderer{m_window}
    , m_input{}
{
    static constexpr std::string_view TEXTURE_PATH = "romfs:/assets/PlayerA.png";

    sdl2::Texture::initialize(m_renderer);

    m_testFont    = sdl2::FontManager::create_load_resource(FONT_PATH, FONT_PATH, 24);
    m_testTexture = sdl2::TextureManager::create_load_resource(TEXTURE_PATH, TEXTURE_PATH);
}

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
        m_testFont->render_text(0, 0, "Test text.");

        m_testTexture->render(0, 32);
        m_renderer.frame_end();
    }
}
