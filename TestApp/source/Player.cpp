#include "Player.hpp"

#include "Bullet.hpp"
#include "Game.hpp"
#include "window.hpp"

namespace
{
    constexpr std::string_view PLAYER_SPRITE_PATH = "romfs:/assets/PlayerA.png";
}

//                      ---- Construction ----

Player::Player()
{
    static constexpr int PLAYER_START_X = 16;

    // Set the sprite.
    Object::set_sprite(sdl2::TextureManager::create_load_resource(PLAYER_SPRITE_PATH, PLAYER_SPRITE_PATH));

    // Set X and Y
    m_x = PLAYER_START_X;
    m_y = (window::LOGICAL_HEIGHT / 2) - (m_height / 2);
}

//                      ---- Public Functions ----

void Player::update(Game *game, const sdl2::Input &input)
{
    // Bullet offsets.
    static constexpr int BULLET_X_OFFSET = 42;
    static constexpr int BULLET_Y_OFFSET = 16;

    // This is the normal speed the ship moves around at.
    static constexpr int STATIC_MOVEMENT = 4;

    // This is to make things harder.
    static constexpr int REVERSE_MOVEMENT = -2;

    // Conditions.
    // These are so the player can't hide off screen.
    const bool canMoveUp    = m_y - STATIC_MOVEMENT > 0;
    const bool canMoveDown  = m_y + m_height < window::LOGICAL_HEIGHT;
    const bool canMoveLeft  = m_x - REVERSE_MOVEMENT > 0;
    const bool canMoveRight = m_x + m_width < window::LOGICAL_WIDTH;

    const bool upPressed    = input.button_pressed(HidNpadButton_AnyUp) || input.button_held(HidNpadButton_AnyUp);
    const bool downPressed  = input.button_pressed(HidNpadButton_AnyDown) || input.button_held(HidNpadButton_AnyDown);
    const bool leftPressed  = input.button_pressed(HidNpadButton_AnyLeft) || input.button_held(HidNpadButton_AnyLeft);
    const bool rightPressed = input.button_pressed(HidNpadButton_AnyRight) || input.button_held(HidNpadButton_AnyRight);
    const bool firePressed  = input.button_pressed(HidNpadButton_A) || input.button_pressed(HidNpadButton_ZL);

    // Whether or not to move.
    const bool moveUp    = canMoveUp && upPressed;
    const bool moveDown  = canMoveDown && downPressed;
    const bool moveLeft  = canMoveLeft && leftPressed;
    const bool moveRight = canMoveRight && rightPressed;

    if (moveUp) { m_y -= STATIC_MOVEMENT; }
    else if (moveDown) { m_y += STATIC_MOVEMENT; }

    if (moveLeft) { m_x += REVERSE_MOVEMENT; }
    else if (moveRight) { m_x += STATIC_MOVEMENT; };

    if (firePressed) { game->create_add_object<Bullet>(m_x + BULLET_X_OFFSET, m_y + BULLET_Y_OFFSET); }
}