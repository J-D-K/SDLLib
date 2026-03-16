#include "Sound.hpp"

#include "Audio.hpp"

//                      ---- Constructor ----

sdl2::Sound::Sound(std::string_view path)
{
    // We're going to take the buffer from SDL2 and wrap it so we don't need to worry about freeing it.
    uint8_t *sdlBuffer{};
    SDL_AudioSpec *audioSpec = SDL_LoadWAV(path.data(), &m_audioSpec, &sdlBuffer, &m_audioLength);
    if (!audioSpec) { return; }

    // Use our unique_ptr to wrap it.
    m_soundBuffer.reset(sdlBuffer);
}

//                      ---- Public Functions ----

void sdl2::Sound::play() const
{
    // Bail if these aren't set.
    if (!sm_audio || !m_soundBuffer) { return; }

    // Queue the sound.
    SDL_QueueAudio(sm_audio->m_audioDevice, m_soundBuffer.get(), m_audioLength);
}

void sdl2::Sound::initialize(sdl2::Audio &audio) { sm_audio = &audio; }