#include "error.hpp"

#include <array>
#include <cstdarg>
#include <string>

namespace
{
    constexpr int SIZE_VA_BUFFER = 0x400;

    std::string s_ErrorString{};
}

static void prep_locations(const std::source_location &location, std::string_view &file, std::string_view &function);
static std::string get_formatted(const char *format, ...);

const char *sdl::error::get_string() { return s_ErrorString.c_str(); }

bool sdl::error::sdl(int result, const std::source_location &location)
{
    if (result == 0) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    s_ErrorString =
        get_formatted("SDL2::%s::%s::%u::%u:%i", file.data(), function.data(), location.line(), location.column(), result);
    return true;
}

bool sdl::error::freetype(int result, const std::source_location &location)
{
    if (result == 0) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    s_ErrorString =
        get_formatted("FreeType::%s::%s::%u::%u:%i", file.data(), function.data(), location.line(), location.column(), result);
    return true;
}

bool sdl::error::libnx(Result result, const std::source_location &location)
{
    if (result == 0) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    s_ErrorString =
        get_formatted("%s::%s::%u::%u::%X", file.data(), function.data(), location.line(), location.column(), result);
    return true;
}

bool sdl::error::is_null(const void *pointer, const std::source_location &location)
{
    if (pointer) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    s_ErrorString = get_formatted("%s::%s::%u::%u::nullptr!", file.data(), function.data(), location.line(), location.column());
    return true;
}

static void prep_locations(const std::source_location &location, std::string_view &file, std::string_view &function)
{
    file                   = location.file_name();
    const size_t fileBegin = file.find_last_of('/');
    if (fileBegin != file.npos) { file = file.substr(fileBegin + 1); }

    function               = location.function_name();
    const size_t funcBegin = file.find_first_of(' ');
    if (funcBegin != function.npos) { function = function.substr(funcBegin + 1); }
}

static std::string get_formatted(const char *format, ...)
{
    std::array<char, SIZE_VA_BUFFER> vaBuffer = {0};

    std::va_list vaList{};
    va_start(vaList, format);
    std::vsnprintf(vaBuffer.data(), SIZE_VA_BUFFER, format, vaList);
    va_end(vaList);

    return std::string(vaBuffer.data());
}
