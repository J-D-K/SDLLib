#include "Background.hpp"

#include <format>

//                      ---- Construction ----

Background::Background()
    : Object(Object::Type::Generic)
{
    // The paths of each background layer.
    static constexpr std::array<std::string_view, 3> LAYER_PATHS = {"romfs:/assets/background_layer_0.png",
                                                                    "romfs:/assets/background_layer_1.png",
                                                                    "romfs:/assets/background_layer_2.png"};

    // Loop em.
    int i{};
    for (std::string_view layer : LAYER_PATHS)
    {
        // Generate layer name.
        const std::string layerName = std::format("BG_LAYER_{}", i++);

        // Struct for vector.
        BackgroundLayer newLayer = {.x = 0, .texture = sdl2::TextureManager::create_load_resource(layerName, layer)};

        // Push it.
        m_layers.push_back(newLayer);
    }
}

//                      ---- Public Functions ----

void Background::update(Game *game, const sdl2::Input &input)
{
    // Scroll the backgrounds according to their position in the vector.
    const size_t vectorSize = m_layers.size();
    for (size_t i = 0; i < vectorSize; i++)
    {
        // Reference to current layer.
        BackgroundLayer &currentLayer = m_layers.at(i);

        // Scroll it.
        currentLayer.x -= i + 1;

        // Loop.
        if (currentLayer.x + currentLayer.texture->get_width() <= 0) { currentLayer.x = 0; }
    }
}

void Background::render(sdl2::Renderer &renderer)
{
    // Loop through layers and render.
    for (BackgroundLayer &layer : m_layers)
    {
        // This makes stuff easier for me.
        const int x                  = layer.x;
        const int width              = layer.texture->get_width();
        sdl2::SharedTexture &texture = layer.texture;

        // First pass.
        texture->render(x, 0);
        texture->render(x + width, 0);
    }
}