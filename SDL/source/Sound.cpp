#include "Sound.hpp"

#include "error.hpp"
#include "sdl.hpp"

sdl::Sound::Sound(std::string_view path)
{
    uint8_t *sdlBuffer{};
    SDL_AudioSpec *result = SDL_LoadWAV(path.data(), &m_audioSpec, &sdlBuffer, &m_length);
    if (!result) { return; }

    m_buffer.reset(sdlBuffer);
    m_isLoaded = true;
}

sdl::Sound::Sound(sdl::Sound &&sound)
    : m_isLoaded(sound.m_isLoaded)
    , m_audioSpec(sound.m_audioSpec)
    , m_length(sound.m_length)
    , m_buffer(std::move(sound.m_buffer)) {};

sdl::Sound &sdl::Sound::operator=(sdl::Sound &&sound)
{
    m_isLoaded  = sound.m_isLoaded;
    m_audioSpec = sound.m_audioSpec;
    m_length    = sound.m_length;
    m_buffer    = std::move(sound.m_buffer);

    return *this;
}

void sdl::Sound::play()
{
    if (!m_isLoaded) { return; }

    const SDL_AudioDeviceID deviceID = sdl::get_audio_device();
    const bool queueError            = sdl::error::sdl(SDL_QueueAudio(deviceID, m_buffer.get(), m_length));
    if (queueError) { return; }

    SDL_PauseAudioDevice(deviceID, 0);
}