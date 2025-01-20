#pragma once
#include "color.hpp"
#include <SDL2/SDL.h>

namespace sdl
{
    /// @brief Wrapper class for SDL_Textures. This shouldn't be used directly. See textureManager.hpp instead.
    class Texture
    {
        public:
            /// @brief Constructor that loads from file.
            /// @param imagePath Path to the image to load.
            Texture(const char *imagePath);

            /// @brief Constructor that creates a texture from a surface.
            /// @param surface Surface to create a texture from.
            /// @param freeSurface Optional. Whether or not the surface should be freed after the texture is created. True by default.
            Texture(SDL_Surface *surface, bool freeSurface = true);

            /// @brief Constructor that loads from a memory buffer.
            /// @param data Pointer to memory to load from.
            /// @param dataSize Size of buffer.
            Texture(const void *data, size_t dataSize);

            /// @brief Constructor that creates a texture with flags.
            /// @param width Width of new texture.
            /// @param height Height of new texture.
            /// @param accessFlags SDL_ACCESSFLAGS_X.
            Texture(int width, int height, int accessFlags);

            /// @brief Destroys the texture upon destruction.
            ~Texture();

            /// @brief Returns the pointer to the underlying SDL_Texture.
            /// @return Pointer the the underlying SDL_Texture.
            SDL_Texture *get(void);

            /// @brief Renders the texture as-is to x and y.
            /// @param target Render target.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @return True on success. False on failure.
            bool render(SDL_Texture *target, int x, int y);

            /// @brief Renders texture stretched.
            /// @param target Render target.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param width Width to render at.
            /// @param height Height to render at.
            /// @return True on success. False on failure.
            bool renderStretched(SDL_Texture *target, int x, int y, int width, int height);

            /// @brief Renders part of the texture.
            /// @param target Render target.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param sourceX X coordinate of the source (this) texture.
            /// @param sourceY Y coordinate of the source (this) texture.
            /// @param sourceWidth Width of area rendered.
            /// @param sourceHeight Height of area rendered.
            /// @return True on success. False on failure.
            bool renderPart(SDL_Texture *target, int x, int y, int sourceX, int sourceY, int sourceWidth, int sourceHeight);

            /// @brief Renders part of the texture stretched.
            /// @param target Render target.
            /// @param sourceX X of source (this).
            /// @param sourceY Y of source (this).
            /// @param sourceWidth Width of area to be rendered.
            /// @param sourceHeight Height of area to be rendered.
            /// @param destinationX X coordinate to render to.
            /// @param destinationY Y coordinate to render to.
            /// @param destinationWidth Width to render at.
            /// @param destinationHeight Height to render at.
            /// @return True on success. False on failure.
            bool renderPartStretched(SDL_Texture *target,
                                     int sourceX,
                                     int sourceY,
                                     int sourceWidth,
                                     int sourceHeight,
                                     int destinationX,
                                     int destinationY,
                                     int destinationWidth,
                                     int destinationHeight);

            /// @brief Clears texture to a color.
            /// @param color Color to clear texture to.
            /// @return True on success. False on failure.
            bool clear(sdl::Color color);

            /// @brief Resizes a texture. Only really useful for render targets.
            /// @param width New width of texture.
            /// @param height New height of texture.
            /// @return True on success. False on failure.
            bool resize(int width, int height);

            /// @brief Sets a color mod for rendering texture with.
            /// @param color Color to use.
            /// @return True on success. False on failure.
            bool setColorMod(sdl::Color color);

        private:
            /// @brief Underlying SDL_Texture.
            SDL_Texture *m_texture = nullptr;

            /// @brief Width and height of texture. Stored here so I don't need to call QueryTexture.
            int m_width = 0, m_height = 0;

            /// @brief Flags texture was created with, if it was created.
            int m_accessFlags = 0;

            /// @brief Ensures that alpha blending is set for the texture.
            void enableBlending(void);

            /// @brief Shortcut call to RenderCopy that logs errors.
            /// @param source Source Rect (coordinates)
            /// @param destination Destination Rect (coordinates)
            /// @return True on success. False on failure.
            bool renderCopy(const SDL_Rect *source, const SDL_Rect *destination);
    };
} // namespace sdl
