#pragma once
#include "color.hpp"
#include <SDL2/SDL.h>

namespace sdl
{
    class Texture
    {
        public:
            // The texture manager class uses a template function and these overloads to create and return textures.
            // Loads texture from path passed.
            Texture(const char *imagePath);
            // Creates a texture from the Surface passed. Free is optional and whether or not this function should free the surface afterwards.
            Texture(SDL_Surface *surface, bool freeSurface = true);
            // Creates a texture from the Data passed.
            Texture(const void *data, size_t dataSize);
            // Creates a blank texture Width and Height in size with AccessFlags.
            Texture(int width, int height, int accessFlags);
            // Frees the texture.
            ~Texture();

            // Returns underlying texture pointer.
            SDL_Texture *get(void);

            // Renders texture as-is to X and Y.
            bool render(SDL_Texture *target, int x, int y);
            // Renders texture stretched to Width and Height at X and Y.
            bool renderStretched(SDL_Texture *target, int x, int y, int width, int height);
            // Renders a part of the texture at X and Y.
            bool renderPart(SDL_Texture *target, int x, int y, int sourceX, int sourceY, int sourceWidth, int sourceHeight);
            // Renders part of the texture stretched a t X and Y with width and height.
            bool renderPartStretched(SDL_Texture *target,
                                     int          sourceX,
                                     int          sourceY,
                                     int          sourceWidth,
                                     int          sourceHeight,
                                     int          destinationX,
                                     int          destinationY,
                                     int          destinationWidth,
                                     int          destinationHeight);

            // Clears texture to ClearColor
            bool clear(sdl::Color color);
            // Resizes the texture to Width and Height with SDLAccessFlags
            bool resize(int width, int height);
            // Sets color mod to texture.
            bool setColorMod(sdl::Color color);

        private:
            // Underlying texture.
            SDL_Texture *m_texture = nullptr;
            // Width and height so I don't need to query for it.
            int m_width = 0, m_height = 0;
            // This is only saved for the constructor that creates textures for render targets.
            int m_accessFlags = 0;
            // This makes sure blending is enabled for transparency.
            void enableBlending(void);
            // This is a shortcut sorta to renderCopy
            bool renderCopy(const SDL_Rect *source, const SDL_Rect *destination);
    };
} // namespace sdl
