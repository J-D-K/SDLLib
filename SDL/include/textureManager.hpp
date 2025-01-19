#pragma once
#include "texture.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace sdl
{
    using SharedTexture = std::shared_ptr<sdl::Texture>;

    // JKSV only uses textures. There's no point in making this generic for more types for now, I guess.
    class TextureManager
    {
        public:
            // There is only one instance and it can't be copied.
            TextureManager(const TextureManager &) = delete;
            TextureManager(TextureManager &&) = delete;
            TextureManager &operator=(const TextureManager &) = delete;
            TextureManager &operator=(TextureManager &&) = delete;

            /*
                Refer to texture.hpp for how this works completely.
                sdl::sharedTexture = sdl::TextureManager::createLoadTexture(TextureName, [Arguments for Texture constructor you're using here.]);
            */
            template <typename... args>
            static sdl::SharedTexture createLoadTexture(std::string_view textureName, args... arguments)
            {
                // This is the pointer we're returning.
                sdl::SharedTexture returnTexture = nullptr;

                // Need the instance to do anything, really.
                TextureManager &manager = TextureManager::getInstance();

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
            // No constructing.
            TextureManager(void) = default;
            static TextureManager &getInstance(void)
            {
                static TextureManager Instance;
                return Instance;
            }
            // Map of weak_ptrs to textures so they free automatically after they expire.
            static inline std::unordered_map<std::string, std::weak_ptr<sdl::Texture>> sm_textureMap;
    };
} // namespace sdl
