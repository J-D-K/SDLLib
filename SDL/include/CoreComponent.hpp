#pragma once

namespace sdl2
{
    class CoreComponent
    {
        public:
            /// @brief Default.
            CoreComponent() = default;

            /// @brief Virtual destructor.
            virtual ~CoreComponent() {};

            /// @brief Returns whether or not initializing the core component was successful.
            bool is_initialized() const noexcept { return m_isInitialized; }

        protected:
            /// @brief Stores whether or not constructing the core component was successful.
            bool m_isInitialized{};
    };
}