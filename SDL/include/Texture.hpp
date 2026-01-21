#pragma once
#include "CoreComponent.hpp"
#include "Renderer.hpp"
#include "Surface.hpp"

#include <SDL2/SDL.h>
#include <string_view>

namespace sdl2
{
    /// @brief Texture class.
    /** @note
     *  Texture::initialize() must be called before textures can be used.
     */
    class Texture final : public sdl2::CoreComponent
    {
        public:
            /// @brief Default constructor.
            Texture() = default;

            /// @brief Loads a texture from the file path passed.
            /// @param filePath Path to load the texture from.
            Texture(std::string_view filePath);

            /// @brief Creates a texture from the surface passed.
            /// @param surface Surface to create the texture from.
            Texture(sdl2::Surface &surface);

            /// @brief Creates a blank texture using the arguments passed.
            /// @param width Width of the texture.
            /// @param height Height of the texture.
            /// @param textureAccess Texture access.
            Texture(int width, int height, SDL_TextureAccess textureAccess);

            /// @brief Destructs the texture.
            ~Texture();

            /// @brief Renders the texture at the coordinates passed.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            bool render(int x, int y);

            /// @brief Renders a part of the texture at the coordinates passed.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param sourceX Source X coordinate of the texture.
            /// @param sourceY Source Y coordinate of the texture.
            /// @param sourceWidth Source width from the texture.
            /// @param sourceHeight Source height from the texture.
            bool render_part(int x, int y, int sourceX, int sourceY, int sourceWidth, int sourceHeight);

            /// @brief Renders the texture stretched at the coordinates passed.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param width Width to render at.
            /// @param height Height to render at.
            bool render_stretched(int x, int y, int width, int height);

            /// @brief Renders part of the texture stretched with the arguments passed.
            /// @param x X coordinate to render to.
            /// @param y Y coordinate to render to.
            /// @param width Width to render at.
            /// @param height Height to render at.
            /// @param sourceX Source X coordinate of the texture.
            /// @param sourceY Source Y coordinate of the texture.
            /// @param sourceWidth Source width of the part.
            /// @param sourceHeight Source height of the part.
            bool render_part_stretched(int x,
                                       int y,
                                       int width,
                                       int height,
                                       int sourceX,
                                       int sourceY,
                                       int sourceWidth,
                                       int sourceHeight);

            /// @brief Initializes the Texture class.
            /// @param renderer Reference to renderer that textures shall belong to.
            static void initialize(const sdl2::Renderer &renderer);

        private:
            /// @brief Underlying SDL_Texture.
            SDL_Texture *m_texture{};

            /// @brief Width of the texture.
            int m_width{};

            /// @brief Height of the texture.
            int m_height{};

            /// @brief Shared pointer to the renderer once it's passed.
            static inline SDL_Renderer *sm_renderer{};
    };
}