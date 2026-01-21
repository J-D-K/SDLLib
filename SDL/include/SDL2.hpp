#pragma once
#include "CoreComponent.hpp"

namespace sdl2
{
    /// @brief This is just a bare-bones wrapper to encapsulate SDL2 init and quit.
    class SDL2 final : public sdl2::CoreComponent
    {
        public:
            /// @brief Initializes SDL2.
            SDL2();

            /// @brief Quits SDL2.
            ~SDL2();
    };
}