#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <string_view>

namespace sdl
{
    /// @brief Forward so this works right.
    class Sound;

    /// @brief This is what should be used instead of raw SDL pointers.
    using SharedSound = std::shared_ptr<sdl::Sound>;

    class Sound final
    {
        public:
            Sound() = default;

            /// @brief Loads a sound file from the path passed.
            Sound(std::string_view path);

            // Allow moving.
            Sound(Sound &&sound) noexcept;
            Sound &operator=(Sound &&sound) noexcept;

            // No copying.
            Sound(const Sound &)            = delete;
            Sound &operator=(const Sound &) = delete;

            /// @brief Plays the sound.
            void play();

        private:
            /// @brief This doesn't need to be exposed outside of here.
            using WavBuffer = std::unique_ptr<uint8_t, decltype(&SDL_FreeWAV)>;

            /// @brief Stores whether or not a sound (wav) file was loaded.
            bool m_isLoaded{};

            /// @brief This is for storing the audio spec.
            SDL_AudioSpec m_audioSpec{};

            /// @brief For storing audio length.
            uint32_t m_length{};

            /// @brief Buffer for the audio data.
            std::unique_ptr<uint8_t, decltype(&SDL_FreeWAV)> m_buffer = WavBuffer(nullptr, SDL_FreeWAV);
    };
}