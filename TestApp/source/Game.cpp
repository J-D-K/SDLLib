#include "Game.hpp"

#include "Bullet.hpp"
#include "Player.hpp"
#include "window.hpp"

#include <format>

namespace
{
    constexpr SDL_Color CLEAR_COLOR = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF};

    constexpr std::string_view SYSTEM_FONT_NAME = "SystemFont";
    constexpr std::string_view FONT_PATH        = "romfs:/assets/MainFont.ttf";
    constexpr int FONT_SIZE                     = 24;
}

//                      ---- Construction ----

Game::Game()
    : m_sdl2{}
    , m_window{window::WIDTH, window::HEIGHT}
    , m_renderer{m_window}
    , m_input{}
{
    // Make renderer use logicals.
    m_renderer.set_logical_presentation(window::LOGICAL_WIDTH, window::LOGICAL_HEIGHT);

    // Init textures.
    sdl2::Texture::initialize(m_renderer);

    // Load the system font.
    m_font = sdl2::FontManager::create_load_resource<sdl2::SystemFont>(SYSTEM_FONT_NAME, 10);

    // Reserve space.
    m_objects.reserve(1024);

    // Create the player.
    Game::create_add_object<Player>();
}

//                      ---- Public Functions ----

int Game::run() noexcept
{
    for (;;)
    {
        // Update input.
        m_input.update();

        // If plus is pressed, break.
        if (m_input.button_pressed(HidNpadButton_Plus)) { return 0; }

        // Update routine.
        Game::update();

        // Render.
        Game::render();
    }
}

//                      ---- Private Functions ----

void Game::update()
{
    // Start by purging object.
    Game::purge_objects();

    if (m_input.button_pressed(HidNpadButton_ZR) || m_input.button_held(HidNpadButton_ZR))
    {
        Game::create_add_object<Bullet>(0, 0);
    }

    // Loop and update.
    for (auto &object : m_objects) { object->update(this, m_input); }
}

void Game::render()
{
    // Color for clearing the target.
    static constexpr SDL_Color BLACK = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0x00};

    // Begin the frame.
    m_renderer.frame_begin(BLACK);

    // Loop and render.
    for (auto &object : m_objects) { object->render(m_renderer); }

    m_font->render_text(0, 0, std::format("Objects: {}", m_objects.size()));

    // Present.
    m_renderer.frame_end();
}

void Game::purge_objects()
{
    for (auto iter = m_objects.begin(); iter != m_objects.end();)
    {
        // If it's purgable, erase it.
        if (iter->get()->is_purgable())
        {
            iter = m_objects.erase(iter);
            continue;
        }

        ++iter;
    }
}