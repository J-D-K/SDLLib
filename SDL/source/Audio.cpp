#include "Audio.hpp"

//                      ---- Construction ----

sdl2::Audio::Audio()
{
    // Audio spec for opening the device.
    static constexpr SDL_AudioSpec AUDIO_SPEC = {.freq     = 11025,
                                                 .format   = AUDIO_U8,
                                                 .channels = 2,
                                                 .silence  = 0,
                                                 .samples  = 256};

    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &AUDIO_SPEC, nullptr, 0);
    if (m_audioDevice == 0) { return; }

    // Unpause audio.
    SDL_PauseAudioDevice(m_audioDevice, 0);

    m_isInitialized = true;
}

sdl2::Audio::~Audio() { SDL_CloseAudioDevice(m_audioDevice); }

//                      ---- Public Functions ----

//                      ---- Private Functions ----