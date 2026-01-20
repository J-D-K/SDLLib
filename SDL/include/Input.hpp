#pragma once
#include <cstdint>
#include <switch.h>

namespace sdl2
{
    /// @brief This isn't an SDL wrapper because this is for switch.
    class Input final
    {
        public:
            /// @brief Constructor. Initializes gamepad.
            Input();

            /// @brief Runs the pad update routine.
            void update() noexcept;

            /// @brief Returns whether or not the button passed was pressed.
            /// @param button Button to check.
            bool button_pressed(HidNpadButton button) noexcept;

            /// @brief Returns whether or not the button passed was held.
            /// @param button Button to check.
            bool button_held(HidNpadButton button) noexcept;

            /// @brief Returns whether or not the button passed was released.
            /// @param button Button to check.
            bool button_released(HidNpadButton button) noexcept;

        private:
            /// @brief Pad state used for input.
            PadState m_padState{};

            /// @brief The current control frame.
            uint64_t m_currentFrame{};

            /// @brief The previous control frame.
            uint64_t m_previousFrame{};

            /// @brief Helper to get the states of both frames.
            /// @param button Button to check.
            /// @param previous Bool for previous frame.
            /// @param current Bool for current frame.
            inline void get_frame_states(HidNpadButton button, bool &previous, bool &current)
            {
                previous = m_previousFrame & button;
                current  = m_currentFrame & button;
            }
    };
}