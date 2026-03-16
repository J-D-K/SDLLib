#pragma once
#include "CoreComponent.hpp"

#include <SDL2/SDL.h>

namespace sdl2
{
    /// @brief Forward to prevent headaches.
    class Sound;

    /// @brief Class for RAII Audio initialization.
    class Audio final : public sdl2::CoreComponent
    {
        public:
            /// @brief Constructor. Opens the audio device.
            Audio();

            /// @brief Destructor.
            ~Audio();

            /// @brief Less headaches.
            friend class Sound;

        private:
            /// @brief Audio device ID.
            SDL_AudioDeviceID m_audioDevice{};
    };
}