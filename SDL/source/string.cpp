#include "string.hpp"

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
}

std::string string::getFormattedString(const char *format, ...)
{
    char vaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    return std::string(vaBuffer);
}
