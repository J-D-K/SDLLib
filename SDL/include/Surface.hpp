#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <span>
#include <string_view>

namespace sdl2
{
    /// @brief Self cleaning wrapper.
    using Surface = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;

    namespace surface
    {
        /// @brief Creates and empty RGB surface with width and height in pixels.
        /// @param width Width of the surface.
        /// @param height Height of the surface.
        inline Surface create_rgb(int width, int height)
        {
            // This is like this for pure readability.
            // Masks for creating the surface.
            static constexpr uint32_t RED_MASK   = 0xFF000000;
            static constexpr uint32_t GREEN_MASK = 0x00FF0000;
            static constexpr uint32_t BLUE_MASK  = 0x0000FF00;
            static constexpr uint32_t ALPHA_MASK = 0x000000FF;

            // Bit depth.
            static constexpr int BIT_DEPTH = 8;

            // I didn't want the return to be too much of a mess.
            SDL_Surface *returnSurface =
                SDL_CreateRGBSurface(0, width, height, BIT_DEPTH, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);

            // If the surface above is nullptr, this will be too.
            return Surface(returnSurface, SDL_FreeSurface);
        }

        /// @brief Returns a surface created from an external file.
        /// @param filePath Path of the image to load.
        inline Surface from_file(std::string_view filePath)
        {
            // Just fall bad to SDL_image for this. Screw using libpng and libjpeg directly...
            return Surface(IMG_Load(filePath.data()), SDL_FreeSurface);
        }

        /// @brief Templated function to allow creating surfaces from various buffer data types.
        /// @tparam Type Type that makes up the buffer.
        /// @param data Surface data.
        template <typename Type>
        inline Surface from_memory(std::span<const Type> data)
        {
            // SDL RWOps.
            SDL_RWops *rwOps = SDL_RWFromConstMem(data.data(), data.size());
            return Surface(IMG_Load_RW(rwOps, 1), SDL_FreeSurface);
        }
    }
}