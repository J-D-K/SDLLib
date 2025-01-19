#pragma once
#include <cstdarg>
#include <string>

// This is only used internally to set the error string. It is not exposed.
namespace string
{
    // Returns a C++ string from a formatted C string.
    std::string getFormattedString(const char *format, ...);
} // namespace string
