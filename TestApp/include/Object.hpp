#pragma once
#include "sdl.hpp"

class Game;

class Object
{
    public:
        /// @brief Default constructor.
        Object() = default;

        /// @brief Virtual destructor.
        virtual ~Object() {};

        /// @brief Virtual Update function.
        /// @param input Reference to input instance.
        virtual void update(Game *game, const sdl2::Input &input) {};

        /// @brief Virtual render function.
        /// @param renderer Reference to renderer.
        virtual void render(sdl2::Renderer &renderer)
        {
            // This is just a generic sprite rendering routine.
            if (!m_sprite->is_initialized()) { return; }

            m_sprite->render(m_x, m_y);
        };

        /// @brief Returns whether or not the object can be purged or destroyed.
        bool is_purgable() const noexcept { return m_isPurgable; }

        /// @brief Marks the object as purgable.
        void set_purgable() noexcept { m_isPurgable = true; }

        /// @brief Sets the sprite of the current Object.
        /// @param sprite Sprite to assign to.
        void set_sprite(sdl2::SharedTexture sprite)
        {
            // Record width and height.
            m_width  = sprite->get_width();
            m_height = sprite->get_height();

            // Assign.
            m_sprite = sprite;
        }

        /// @brief Returns if there is a basic bounding box collision.
        /// @param object Object to check.
        /// @return True on collision. False on not.
        bool collision(const Object &object)
        {
            // We're going to actually check if there isn't a collision and invert the result.
            const bool noXCollision = m_x + m_width < object.m_x && m_x > object.m_x + object.m_width;
            const bool noYCollision = m_y + m_height < object.m_y && m_y > object.m_y + object.m_height;

            return !noXCollision && !noYCollision;
        }

    protected:
        /// @brief X coordinate.
        int m_x{};

        /// @brief Y coordinate.
        int m_y{};

        /// @brief Width of the object.
        int m_width{};

        /// @brief Height of the object.
        int m_height{};

        /// @brief Stores whether or not the object has served its purpose.
        bool m_isPurgable{};

        /// @brief Sprite for rendering.
        sdl2::SharedTexture m_sprite{};
};