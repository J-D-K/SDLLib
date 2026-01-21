#pragma once
#include "Texture.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace sdl2
{
    /// @brief Shared Texture definition.
    using SharedTexture = std::shared_ptr<Texture>;

    template <typename ResourceType>
    class ResourceManager final
    {
        public:
            // No copying or moving.
            ResourceManager(const ResourceManager &)            = delete;
            ResourceManager(ResourceManager &&)                 = delete;
            ResourceManager &operator=(const ResourceManager &) = delete;
            ResourceManager &operator=(ResourceManager &&)      = delete;

            /// @brief Templated resource creation/loading function that passed parameters to the constructor.
            /// @param name Internal "name" of the resource.
            /// @param ...args Variadic arguments to forward to the constructor.
            template <typename... Args>
            std::shared_ptr<ResourceType> create_load_resource(std::string_view name, Args &&...args)
            {
                // Grab the instance.
                ResourceManager &instance = ResourceManager::get_instance();

                // Run purge routine.
                instance.purge_expired();

                // Reference to map.
                auto &resourceMap = instance.m_resourceMap;

                // Search for resource.
                const auto &findResource = resourceMap.find(name);
                if (findResource != resourceMap.end() && !findResource->second.expired())
                {
                    return findResource->second.lock();
                }
                else
                {
                    // Create the resource.
                    auto resource = std::make_shared<ResourceType>(std::forward<Args>(args)...);

                    // Map it.
                    resourceMap.try_emplace(std::string{name}, resource);

                    // Return it.
                    return resource;
                }

                return nullptr;
            }

        private:
            // clang-format off
            struct StringViewHash
            {
                using is_transparent = void;
                size_t operator() (std::string_view view) const noexcept { return std::hash<std::string_view>{}(view); }
            };

            struct StringViewEquals
            {
                using is_transparent = void;
                bool operator() (std::string_view viewA, std::string_view viewB) const noexcept { return viewA == viewB; }
            };
            // clang-format on

            /// @brief Map with weak pointers to resources.
            std::unordered_map<std::string, std::weak_ptr<ResourceType>, StringViewHash, StringViewEquals> m_resourceMap{};

            /// @brief Private constructor.
            ResourceManager() = default;

            /// @brief Returns the instance.
            ResourceManager &get_instance()
            {
                static ResourceManager instance;
                return instance;
            }

            /// @brief Purges expired resources from the map.
            void purge_expired()
            {
                for (auto iter = m_resourceMap.begin(); iter != m_resourceMap.end();)
                {
                    // Weak ptr.
                    const auto &weakPtr = iter->second;

                    // If it's expired, purge it.
                    if (weakPtr.expired())
                    {
                        iter = m_resourceMap.erase(iter);
                        continue;
                    }

                    // Increment iterator.
                    ++iter;
                }
            }
    };

    /// @brief Texture manager instance.
    using TextureManager = ResourceManager<Texture>;
}