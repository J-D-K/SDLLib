#pragma once
#include "Texture.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace sdl
{
    /// @brief Static class that keeps track of textures so I don't need to worry about memory leaks.
    class TextureManager
    {
        public:
            TextureManager(const TextureManager &)            = delete;
            TextureManager(TextureManager &&)                 = delete;
            TextureManager &operator=(const TextureManager &) = delete;
            TextureManager &operator=(TextureManager &&)      = delete;

            /// @brief Searches for and either returns or creates a new sdl::Sharedtexture.
            /// @param textureName Name of the texture. This is needed to map it and keep track of it.
            /// @param arguments Arguments of the constructor used. See texture.hpp for that.
            /// @return sdl::SharedTexture.
            template <typename... Args>
            static sdl::SharedTexture load(std::string_view textureName, Args &&...args)
            {
                // This is the pointer we're returning.
                sdl::SharedTexture returnTexture{};

                // Need the instance to do anything, really.
                TextureManager &manager = TextureManager::get_instance();
                auto &m_textureMap      = manager.m_textureMap;

                auto findTexture   = m_textureMap.find(textureName);
                const bool exists  = findTexture != m_textureMap.end();
                const bool expired = exists && findTexture->second.expired();
                if (exists && !expired) { returnTexture = findTexture->second.lock(); }
                else
                {
                    returnTexture                    = std::make_shared<sdl::Texture>(args...);
                    m_textureMap[textureName.data()] = returnTexture;
                }

                return returnTexture;
            }

            template <typename... Args>
            static sdl::SharedTexture replace(std::string_view textureName, Args &&...args)
            {
                sdl::SharedTexture returnTexture{};

                TextureManager &manager = TextureManager::get_instance();
                auto &m_textureMap      = manager.m_textureMap;

                auto findTexture   = m_textureMap.find(textureName);
                const bool exists  = findTexture != m_textureMap.end();
                const bool expired = exists && findTexture->second.expired();
                if (exists && !expired)
                {
                    returnTexture                    = findTexture->second.lock();
                    returnTexture                    = std::make_shared<sdl::Texture>(args...);
                    m_textureMap[textureName.data()] = returnTexture;
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

            void purge_expired()
            {
                for (auto &[name, pointer] : m_textureMap)
                {
                    if (pointer.expired()) { m_textureMap.erase(name); }
                }
            }

            // clang-format off
            struct StringViewHash
            {
                using is_transparent = void;
                size_t operator()(std::string_view view) const noexcept { return std::hash<std::string_view>{}(view); }
            };

            struct StringViewEquals
            {
                using is_transparent = void;
                bool operator()(std::string_view viewA, std::string_view viewB) const noexcept { return viewA == viewB; }
            };
            // clang-format on

            /// @brief Map of weak_ptrs to textures so they are freed automatically once they're not needed anymore.
            std::unordered_map<std::string, std::weak_ptr<sdl::Texture>, StringViewHash, StringViewEquals> m_textureMap;
    };
} // namespace sdl
