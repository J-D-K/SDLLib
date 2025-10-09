#pragma once
#include "Sound.hpp"
#include "Texture.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace sdl
{
    /// @brief Static class that keeps track of resources so I don't need to worry about memory leaks.
    template <typename ResourceType>
    class ResourceManager
    {
        public:
            ResourceManager(const ResourceManager &)            = delete;
            ResourceManager(ResourceManager &&)                 = delete;
            ResourceManager &operator=(const ResourceManager &) = delete;
            ResourceManager &operator=(ResourceManager &&)      = delete;

            /// @brief Searches for and either returns or creates a new sdl::Sharedtexture.
            /// @param textureName Name of the texture. This is needed to map it and keep track of it.
            /// @param arguments Arguments of the constructor used. See texture.hpp for that.
            /// @return sdl::SharedTexture.
            template <typename... Args>
            static std::shared_ptr<ResourceType> load(std::string_view resourceName, Args &&...args)
            {
                // This is the pointer we're returning.
                std::shared_ptr<ResourceType> returnResource{};

                // Need the instance to do anything, really.
                ResourceManager &manager = ResourceManager::get_instance();
                auto &resourceMap        = manager.m_resourceMap;

                // This will purge expired resources/weak_ptrs from the map.
                manager.purge_expired();

                auto findResource  = resourceMap.find(resourceName);
                const bool exists  = findResource != resourceMap.end();
                const bool expired = exists && findResource->second.expired();
                if (exists && !expired) { returnResource = findResource->second.lock(); }
                else
                {
                    returnResource = std::make_shared<ResourceType>(std::forward<Args>(args)...);
                    resourceMap.emplace(std::string{resourceName}, returnResource);
                }

                return returnResource;
            }

        private:
            /// @brief Prevents creating an instance of this class.
            ResourceManager() = default;

            /// @brief Returns the only instance of the class. Private since it shouldn't be used outside of this class.
            /// @return Reference to the static instance of this class.
            static ResourceManager &get_instance()
            {
                static ResourceManager Instance;
                return Instance;
            }

            /// @brief Purges expired resources from the map.
            void purge_expired()
            {
                for (auto iter = m_resourceMap.begin(); iter != m_resourceMap.end();)
                {
                    if (iter->second.expired())
                    {
                        iter = m_resourceMap.erase(iter);
                        continue;
                    }
                    ++iter;
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
            std::unordered_map<std::string, std::weak_ptr<ResourceType>, StringViewHash, StringViewEquals> m_resourceMap;
    };

    /// @brief This is the instance for Textures/images
    using TextureManager = sdl::ResourceManager<sdl::Texture>;

    /// @brief This is the instance for sounds.
    using SoundManager = sdl::ResourceManager<sdl::Sound>;
} // namespace sdl
