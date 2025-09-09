#pragma once
#include "Color.hpp"
#include "Surface.hpp"

#include <SDL2/SDL.h>

namespace sdl
{
    /// @brief Forward declaration so this works right.
    class Texture;

    /// @brief This is what should be used along with the TextureManager.
    using SharedTexture = std::shared_ptr<sdl::Texture>;

    /// @brief Wrapper class for SDL_Textures. This shouldn't be used directly. See TextureManager.hpp instead.
    class Texture
    {
        public:
            /// @brief Default constructor. This should never be used. It's only for the Null texture.
            Texture() = default;

            /// @brief Constructor that loads from file.
            /// @param imagePath Path to the image to load.
            Texture(const char *imagePath);

            /// @brief Constructor that creates a texture from a surface.
            /// @param surface Surface to create a texture from.
            /// @param freeSurface Optional. Whether or not the surface should be freed after the texture is created. True by
            /// default.
            Texture(sdl::Surface &surface);

            /// @brief Constructor that loads from a memory buffer.
            /// @param data Pointer to memory to load from.
            /// @param dataSize Size of buffer.
            Texture(const void *data, size_t dataSize);

            /// @brief Constructor that creates a texture with flags.
            /// @param width Width of new texture.
            /// @param height Height of new texture.
            /// @param accessFlags SDL_ACCESSFLAGS_X.
            Texture(int width, int height, int accessFlags);

            Texture(Texture &&texture) noexcept;
            Texture &operator=(Texture &&texture) noexcept;

            Texture(const Texture &)            = delete;
            Texture &operator=(const Texture &) = delete;

            /// @brief Destroys the texture upon destruction.
            ~Texture();

            /// @brief Returns the underlying SDL_Texture;
            SDL_Texture *get() noexcept;

            /// @brief Renders the texture as-is to x and y.
            /// @param target Render target.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @return True on success. False on failure.
            bool render(sdl::SharedTexture &target, int x, int y) noexcept;

            /// @brief Renders texture stretched.
            /// @param target Render target.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param width Width to render at.
            /// @param height Height to render at.
            /// @return True on success. False on failure.
            bool render_stretched(sdl::SharedTexture &target, int x, int y, int width, int height) noexcept;

            /// @brief Renders part of the texture.
            /// @param target Render target.
            /// @param x X coordinate.
            /// @param y Y coordinate.
            /// @param sourceX X coordinate of the source (this) texture.
            /// @param sourceY Y coordinate of the source (this) texture.
            /// @param sourceWidth Width of area rendered.
            /// @param sourceHeight Height of area rendered.
            /// @return True on success. False on failure.
            bool render_part(sdl::SharedTexture &target,
                             int x,
                             int y,
                             int sourceX,
                             int sourceY,
                             int sourceWidth,
                             int sourceHeight) noexcept;

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
            bool render_part_stretched(sdl::SharedTexture &target,
                                       int sourceX,
                                       int sourceY,
                                       int sourceWidth,
                                       int sourceHeight,
                                       int destinationX,
                                       int destinationY,
                                       int destinationWidth,
                                       int destinationHeight) noexcept;

            /// @brief Clears texture to a color.
            /// @param color Color to clear texture to.
            /// @return True on success. False on failure.
            bool clear(sdl::Color color) noexcept;

            /// @brief Returns the width of the texture.
            int get_width() const noexcept;

            /// @brief Returns the height of the texture.
            int get_height() const noexcept;

            /// @brief Sets a color mod for rendering texture with.
            /// @param color Color to use.
            /// @return True on success. False on failure.
            bool set_color_mod(sdl::Color color) noexcept;

            /// @brief Null texture to pass for Framebuffer access.
            static inline sdl::SharedTexture Null = std::make_shared<sdl::Texture>();

        private:
            /// @brief Underlying SDL_Texture.
            SDL_Texture *m_texture{};

            /// @brief Width and height of texture. Stored here so I don't need to call QueryTexture.
            int m_width{};
            int m_height{};

            /// @brief Flags texture was created with, if it was created.
            int m_accessFlags{};

            /// @brief Ensures that alpha blending is set for the texture.
            void enable_blending() noexcept;

            /// @brief Shortcut call to RenderCopy that logs errors.
            /// @param source Source Rect (coordinates)
            /// @param destination Destination Rect (coordinates)
            /// @return True on success. False on failure.
            bool render_copy(const SDL_Rect *source, const SDL_Rect *destination) noexcept;
    };
} // namespace sdl
