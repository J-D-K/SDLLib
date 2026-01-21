#pragma once
#include "Object.hpp"

#include <vector>

class Background final : public Object
{
    public:
        /// @brief Constructs the background.
        Background();

        /// @brief Scrolls the background.
        /// @param game Pointer to game.
        /// @param input Reference to input instance.
        void update(Game *game, const sdl2::Input &input) override;

        /// @brief Render override.
        /// @param renderer Reference to renderer.
        void render(sdl2::Renderer &renderer) override;

    private:
        // clang-format off
        struct BackgroundLayer
        {
            int x{};
            sdl2::SharedTexture texture{};
        };
        // clang-format on

        /// @brief Background layers.
        std::vector<BackgroundLayer> m_layers{};
};