#pragma once
#include <source_location>
#include <switch.h>

namespace sdl
{
    namespace error
    {
        /// @brief Returns the internal error string.
        const char *get_string() noexcept;

        /// @brief Records an SDL error to the internal string.
        /// @param result Result of the SDL function.
        /// @param location Optional. Records the location in source in which the error occurred.
        /// @return True if an error occurred. False if not.
        bool sdl(int result, const std::source_location &location = std::source_location::current()) noexcept;

        /// @brief Records a freetype error.
        /// @param result Result of the freetype function.
        /// @param location Optional. Records location.
        bool freetype(int result, const std::source_location &location = std::source_location::current()) noexcept;

        /// @brief Records a libnx error to string.
        /// @param result Result to record.
        /// @param location Source location.
        bool libnx(Result result, const std::source_location &location = std::source_location::current()) noexcept;

        bool is_null(const void *pointer, const std::source_location &location = std::source_location::current()) noexcept;
    }
}
