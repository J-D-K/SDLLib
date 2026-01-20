#include "Input.hpp"

namespace
{
    /// @brief Maximum number of players to configure for.
    constexpr uint32_t MAX_PLAYERS = 1;
}

//                      ---- Construction ----

sdl2::Input::Input()
{
    // Config input.
    padConfigureInput(MAX_PLAYERS, HidNpadStyleSet_NpadStandard);

    // Init pad state.
    padInitializeDefault(&m_padState);
}

//                      ---- Public Functions ----

void sdl2::Input::update() noexcept
{
    // Update padState.
    padUpdate(&m_padState);

    // Flip.
    m_previousFrame = m_currentFrame;

    // Assign.
    m_currentFrame = m_padState.buttons_cur;
}

bool sdl2::Input::button_pressed(HidNpadButton button) noexcept
{
    // Bools to store data.
    bool previousFrame{};
    bool currentFrame{};

    // The the frame states.
    Input::get_frame_states(button, previousFrame, currentFrame);

    // Return condition.
    return !previousFrame && currentFrame;
}

bool sdl2::Input::button_held(HidNpadButton button) noexcept
{
    bool previousFrame{};
    bool currentFrame{};

    Input::get_frame_states(button, previousFrame, currentFrame);

    return previousFrame && currentFrame;
}

bool sdl2::Input::button_released(HidNpadButton button) noexcept
{
    bool previousFrame{};
    bool currentFrame{};

    Input::get_frame_states(button, previousFrame, currentFrame);

    return previousFrame && !currentFrame;
}