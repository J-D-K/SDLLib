#pragma once
#include "Font.hpp"
#include "PlService.hpp"

#include <array>
#include <switch.h>

namespace sdl2
{
    class SystemFont final : public Font
    {
        public:
            /// @brief Creates a new system font instance.
            /// @param pixelSize Vertical size of the font in pixels.
            SystemFont(int pixelSize);

            /// @brief Destructor. Frees faces.
            ~SystemFont();

        private:
            /// @brief Total number of faces loaded from the system.
            int m_total{};

            /// @brief Array of font faces.
            std::array<FT_Face, PlSharedFontType_Total> m_fontFaces{};

            /// @brief Static instance shared by all instances.
            static inline PlService sm_plService{};

            /// @brief Override for using the SystemFont instead of an external one.
            /// @param codepoint Codepoint to search for.
            OptionalReference<Font::GlyphData> find_load_glyph(uint32_t codepoint) override;
    };
}