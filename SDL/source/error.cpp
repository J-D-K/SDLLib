#include "error.hpp"

#include <array>
#include <cstdarg>
#include <string>

namespace
{
    constexpr int SIZE_VA_BUFFER = 0x400;

    constinit char s_errorBuffer[SIZE_VA_BUFFER] = {0};
}

static void prep_locations(const std::source_location &location, std::string_view &file, std::string_view &function) noexcept;

const char *sdl::error::get_string() noexcept { return s_errorBuffer; }

bool sdl::error::sdl(int result, const std::source_location &location) noexcept
{
    if (result == 0) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    std::snprintf(s_errorBuffer,
                  SIZE_VA_BUFFER,
                  "SDL2::%s::%s::%u::%u:%i",
                  file.data(),
                  function.data(),
                  location.line(),
                  location.column(),
                  result);

    return true;
}

bool sdl::error::freetype(int result, const std::source_location &location) noexcept
{
    if (result == 0) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    std::snprintf(s_errorBuffer,
                  SIZE_VA_BUFFER,
                  "FreeType::%s::%s::%u::%u:%i",
                  file.data(),
                  function.data(),
                  location.line(),
                  location.column(),
                  result);

    return true;
}

bool sdl::error::libnx(Result result, const std::source_location &location) noexcept
{
    if (result == 0) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    std::snprintf(s_errorBuffer,
                  SIZE_VA_BUFFER,
                  "%s::%s::%u::%u::%X",
                  file.data(),
                  function.data(),
                  location.line(),
                  location.column(),
                  result);

    return true;
}

bool sdl::error::is_null(const void *pointer, const std::source_location &location) noexcept
{
    if (pointer) { return false; }

    std::string_view file{}, function{};
    prep_locations(location, file, function);
    std::snprintf(s_errorBuffer,
                  SIZE_VA_BUFFER,
                  "%s::%s::%u::%u::nullptr!",
                  file.data(),
                  function.data(),
                  location.line(),
                  location.column());

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
