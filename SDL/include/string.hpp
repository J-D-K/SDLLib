#pragma once
#include <cstdarg>
#include <string>

namespace string
{
    /// @brief Takes in a va_list style format string and returns the output as a C++ string.
    /// @param format Format of string.
    /// @param Arguments
    /// @return Formatted C++ string.
    std::string getFormattedString(const char *format, ...);
} // namespace string
