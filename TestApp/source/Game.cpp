#include "Game.hpp"

#include "Background.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "random.hpp"
#include "window.hpp"

#include <cstdlib>
#include <ctime>
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
    // Seed the random generator. This is one of those things I hate C++ for.
    std::srand(std::time(nullptr));

    // Make renderer use logicals.
    m_renderer.set_logical_presentation(window::LOGICAL_WIDTH, window::LOGICAL_HEIGHT);

    // Init textures.
    sdl2::Texture::initialize(m_renderer);

    // Load the system font.
    m_font = sdl2::FontManager::create_load_resource<sdl2::SystemFont>(SYSTEM_FONT_NAME, 10);

    // Reserve space.
    m_objects.reserve(1024);

    // Create the background.
    Game::create_add_object<Background>();

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

void Game::add_to_score(int add) noexcept { m_score += add; }

std::span<const UniqueObject> Game::get_game_objects() const noexcept { return std::span<const UniqueObject>{m_objects}; }

//                      ---- Private Functions ----

void Game::update()
{
    // Start by purging object.
    Game::purge_objects();

    // Roll for enemy spawn.
    const bool spawnEnemy = generate_random(99) <= m_level;
    if (spawnEnemy) { Game::create_add_object<Enemy>(); }

    // Loop and update.
    for (auto &object : m_objects) { object->update(this, m_input); }
}

void Game::render()
{
    // Color for clearing the target.
    static constexpr SDL_Color BLACK = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0x00};
    static constexpr SDL_Color WHITE = {.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF};

    // Begin the frame.
    m_renderer.frame_begin(BLACK);

    // Loop and render.
    for (auto &object : m_objects) { object->render(m_renderer); }

    m_font->render_text(0, 0, WHITE, std::format("Objects: {}\nScore: {}\nLevel: {}", m_objects.size(), m_score, m_level));

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