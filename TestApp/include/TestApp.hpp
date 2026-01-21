#pragma once
#include "sdl.hpp"

class TestApp final
{
    public:
        /// @brief Constructor.
        TestApp();

        /// @brief Runs the application.
        int run() noexcept;

    private:
        /// @brief SDL2 instance.
        sdl2::SDL2 m_sdl2;

        /// @brief Window instance.
        sdl2::Window m_window;

        /// @brief Renderer instance.
        sdl2::Renderer m_renderer;

        /// @brief Input instance.
        sdl2::Input m_input;

        /// @brief Font to test text rendering.
        sdl2::SharedFont m_testFont{};

        /// @brief Test texture.
        sdl2::SharedTexture m_testTexture{};
};