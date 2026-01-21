#include "Bullet.hpp"

#include "window.hpp"

//                      ---- Construction ----

Bullet::Bullet(int x, int y)
{
    // Path to load the sprite from.
    static constexpr std::string_view BULLET_PATH = "romfs:/assets/BulletA.png";

    // Set x and y.
    m_x = x;
    m_y = y;

    // Load sprite.
    Object::set_sprite(sdl2::TextureManager::create_load_resource(BULLET_PATH, BULLET_PATH));
}

//                      ---- Public Functions ----

void Bullet::update(Game *game, const sdl2::Input &input)
{
    // Speed the bullet travels at.
    static constexpr int BULLET_SPEED = 8;

    // Update the X.
    m_x += BULLET_SPEED;

    // If the bullet is off the screen, mark it as purgable.
    if (m_x > window::LOGICAL_WIDTH) { Object::set_purgable(); }
}