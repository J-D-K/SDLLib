#pragma once
#include "Object.hpp"

class Bullet final : public Object
{
    public:
        /// @brief Constructor. Loads the sprite.
        /// @param x X coordinate to construct at.
        /// @param y Y coordinate to construct at.
        Bullet(int x, int y);

        /// @brief Runs the update routine. Moves the bullet.
        /// @param game Reference to game.
        /// @param input Reference to input instance.
        void update(Game *game, const sdl2::Input &input) override;
};