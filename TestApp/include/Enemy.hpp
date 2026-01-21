#pragma once
#include "Object.hpp"
#include "sdl.hpp"

class Enemy final : public Object
{
    public:
        // clang-format off
        struct EnemyData final
        {
            /// @brief Number of hits before the enemy is dead.
            int hitpoints{};
            
            /// @brief Speed of the enemy.
            int speed{};

            /// @brief Score points for the enemy.
            int score{};

            /// @brief Path of the sprite.
            std::string_view spritePath{};
        };
        // clang-format on

        /// @brief Generates a new enemy using random numbers.
        Enemy();

        /// @brief Update routine.
        /// @param game Reference to game.
        /// @param input Reference to input.
        void update(Game *game, const sdl2::Input &input) override;

    private:
        // Hits before death.
        int m_hitpoints{};

        /// @brief Pointer to enemy data.
        EnemyData *m_enemyData{};

        /// @brief Font used to render enemy HP above them.
        static inline sdl2::SharedFont sm_enemyFont{};

        /// @brief Loads the system font for use.
        void initialize_static_members();
};