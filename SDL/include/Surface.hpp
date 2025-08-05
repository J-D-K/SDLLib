#include <SDL2/SDL.h>
#include <memory>
#include <span>
#include <string_view>

namespace sdl
{
    /// @brief This is a wrapper. This isn't getting as much care as texture.
    using Surface = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;

    namespace surface
    {
        /// @brief Creates an empty RGB surface width x height pixels.
        /// @param width Width in pixels.
        /// @param height Height in pixels.
        sdl::Surface create_rgb(int width, int height);

        /// @brief Loads a file into the surface
        /// @param filePath Path of the file to load.
        sdl::Surface from_file(std::string_view filePath);

        /// @brief Creates a surface from a memory pointer.
        /// @param data Data to create the surface from.
        /// @param dataSize Size of the data.
        sdl::Surface from_memory(const void *data, size_t dataSize);
    }
}
