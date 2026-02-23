#pragma once
#include "Object.hpp"
#include "sdl.hpp"

#include <fstream>
#include <span>
#include <vector>

class Game final
{
    public:
        /// @brief Constructor.
        Game();

        /// @brief Runs the application.
        int run() noexcept;

        /// @brief Adds the passed amount to the score count.
        void add_to_score(int add) noexcept;

        /// @brief Returns a span of all of the in game objects.
        std::span<const UniqueObject> get_game_objects() const noexcept;

        /// @brief Templated function to make adding objects to the game easier.
        /// @tparam ObjectType Type of object to add.
        /// @tparam ...Args Arguments to forward to the constructor of the object.
        template <typename ObjectType, typename... Args>
        void create_add_object(Args &&...args)
        {
            // Asset it's a derived class.
            static_assert(std::derived_from<ObjectType, Object> == true, "Game: ObjectType is not derived from Object!");

            // Create a new instance.
            auto newObject = std::make_unique<ObjectType>(std::forward<Args>(args)...);

            // Push it back.
            m_objects.push_back(std::move(newObject));
        }

    private:
        /// @brief SDL2 instance.
        sdl2::SDL2 m_sdl2;

        /// @brief Window instance.
        sdl2::Window m_window;

        /// @brief Renderer instance.
        sdl2::Renderer m_renderer;

        /// @brief Input instance.
        sdl2::Input m_input;

        /// @brief System font.
        sdl2::SharedFont m_font{};

        /// @brief Vector of objects.
        std::vector<UniqueObject> m_objects;

        /// @brief Current score for the game.
        int m_score{};

        /// @brief This is the "level". Really used as a spawn chance.
        int m_level{1};

        /// @brief Runs the update routine.
        void update();

        /// @brief Runs the render routine.
        void render();

        /// @brief Purges expired objects from the vector.
        void purge_objects();
};