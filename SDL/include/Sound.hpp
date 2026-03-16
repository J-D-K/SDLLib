#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <string_view>

namespace sdl2
{
    /// @brief Less headaches.
    class Audio;

    /// @brief SDL Sound wrapper. Should not be used directly. Use SoundManager instead.
    class Sound final
    {
        public:
            /// @brief Loads a new wav sound from the path passed.
            Sound(std::string_view path);

            /// @brief Plays the sound.
            void play() const;

            /// @brief Initializes Sound so it has a pointer to audio.
            /// @param audio Reference to Audio instance.
            static void initialize(sdl2::Audio &audio);

        private:
            /// @brief Definition of sound buffer.
            using SoundBuffer = std::unique_ptr<uint8_t[], decltype(&SDL_FreeWAV)>;

            /// @brief Buffer for sound data.
            SoundBuffer m_soundBuffer = SoundBuffer(nullptr, SDL_FreeWAV);

            /// @brief Stores the audio spec.
            SDL_AudioSpec m_audioSpec{};

            /// @brief The length of the sound.
            uint32_t m_audioLength{};

            /// @brief Shared pointer to the Audio instance.
            static inline sdl2::Audio *sm_audio{};
    };
}