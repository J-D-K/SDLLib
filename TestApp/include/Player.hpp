#pragma once
#include "Object.hpp"

class Player final : public Object
{
    public:
        /// @brief Constructor. Initializes player.
        Player();

        /// @brief Runs the update routine.
        /// @param game Reference to main game class.
        /// @param input Input instance.
        void update(Game *game, const sdl2::Input &input) override;

        // We're just going to use the base render for this.
};