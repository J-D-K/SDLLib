#pragma once
#include "Texture.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace sdl
{
    /// @brief Definition of type so I don't hurt my poor hands typing too much.
    using SharedTexture = std::shared_ptr<sdl::Texture>;

    /// @brief Static class that keeps track of textures so I don't need to worry about memory leaks.
    class TextureManager
    {
        public:
            /// @brief Prevents copying the manage.
            TextureManager(const TextureManager &) = delete;
            /// @brief Prevents copying the manage.
            TextureManager(TextureManager &&) = delete;
            /// @brief Prevents copying the manage.
            TextureManager &operator=(const TextureManager &) = delete;
            /// @brief Prevents copying the manage.
            TextureManager &operator=(TextureManager &&) = delete;

            /// @brief Searches for and either returns or creates a new sdl::Sharedtexture.
            /// @param textureName Name of the texture. This is needed to map it and keep track of it.
            /// @param arguments Arguments of the constructor used. See texture.hpp for that.
            /// @return sdl::SharedTexture.
            template <typename... args>
            static sdl::SharedTexture create_load_texture(std::string_view textureName, args... arguments)
            {
                // This is the pointer we're returning.
                sdl::SharedTexture returnTexture = nullptr;

                // Need the instance to do anything, really.
                TextureManager &manager = TextureManager::get_instance();

                // Search the map first to see if the texture is already loaded and that the pointer hasn't expired.
                if (manager.sm_textureMap.find(textureName.data()) != manager.sm_textureMap.end() &&
                    !manager.sm_textureMap.at(textureName.data()).expired())
                {
                    // Get a lock on it so it can't expired.
                    returnTexture = manager.sm_textureMap.at(textureName.data()).lock();
                }
                else
                {
                    // Load it since it doesn't exist or expired and map it.
                    returnTexture = std::make_shared<sdl::Texture>(arguments...);
                    // Add it to map.
                    manager.sm_textureMap[textureName.data()] = returnTexture;
                }
                return returnTexture;
            }

        private:
            /// @brief Prevents creating an instance of this class.
            TextureManager() = default;

            /// @brief Returns the only instance of the class. Private since it shouldn't be used outside of this class.
            /// @return Reference to the static instance of this class.
            static TextureManager &get_instance()
            {
                static TextureManager Instance;
                return Instance;
            }

            /// @brief Map of weak_ptrs to textures so they are freed automatically once they're not needed anymore.
            static inline std::unordered_map<std::string, std::weak_ptr<sdl::Texture>> sm_textureMap;
    };
} // namespace sdl
