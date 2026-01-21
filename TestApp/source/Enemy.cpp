#include "Enemy.hpp"

#include "Game.hpp"
#include "random.hpp"
#include "window.hpp"

#include <array>
#include <format>

namespace
{
    /// @brief Total number of enemies.
    constexpr size_t ENEMY_TOTAL = 5;

    /// @brief Array of enemy data. These are arranged according to sprite size.
    std::array<Enemy::EnemyData, ENEMY_TOTAL> ENEMY_DATA_ARRAY = {{{1, 10, 100, "romfs:/assets/EnemyA.png"},
                                                                   {2, 8, 200, "romfs:/assets/EnemyC.png"},
                                                                   {3, 7, 300, "romfs:/assets/EnemyB.png"},
                                                                   {4, 6, 400, "romfs:/assets/EnemyE.png"},
                                                                   {8, 4, 800, "romfs:/assets/EnemyD.png"}}};

}

//                      ---- Construction ----

Enemy::Enemy()
    : Object(Object::Type::Enemy)
{
    Enemy::initialize_static_members();

    // Generate the index we're going to use.
    const int enemyIndex = generate_random(ENEMY_TOTAL);

    // Assign local pointer.
    m_enemyData = &ENEMY_DATA_ARRAY.at(enemyIndex);

    // Load the sprite.
    Object::set_sprite(sdl2::TextureManager::create_load_resource(m_enemyData->spritePath, m_enemyData->spritePath));

    // Assign hitpoints.
    m_hitpoints = m_enemyData->hitpoints;

    // Position. X is always off screen.
    m_x = window::LOGICAL_WIDTH + generate_random(window::LOGICAL_WIDTH);
    m_y = generate_random(window::LOGICAL_HEIGHT - m_height);
}

//                      ---- Public Functions ----

void Enemy::update(Game *game, const sdl2::Input &input)
{
    // Just update the position.
    m_x -= m_enemyData->speed;

    // Check for collisions with bullets for now.
    std::span<const UniqueObject> objectSpan = game->get_game_objects();
    for (const UniqueObject &object : objectSpan)
    {
        // Type check. Don't bother collision checking unless it's a bullet.
        if (object->get_type() != Object::Type::Bullet) { continue; }

        // If there's a collision, mark as purgable and move on.
        if (Object::collision(object))
        {
            // Mark the bullet as destructable.
            object->set_purgable();

            // Decrease hitpoints. If they're gone, destruct.
            --m_hitpoints;
            if (m_hitpoints <= 0)
            {
                game->add_to_score(m_enemyData->score);
                Object::set_purgable();
            }
        }
    }

    // Mark for purging if we're off screen.
    if (m_x + m_width < 0) { Object::set_purgable(); }
}

//                      ---- Private Functions ----

void Enemy::initialize_static_members()
{
    static constexpr std::string_view ENEMY_FONT_NAME = "EnemyFont";

    if (sm_enemyFont) { return; }

    sm_enemyFont = sdl2::FontManager::create_load_resource<sdl2::SystemFont>(ENEMY_FONT_NAME, 8);
}