#pragma once
#include "GlyphData.hpp"

#include <array>
#include <map>
#include <optional>
#include <switch.h>

// clang-format off
#include <ft2build.h>
#include FT_FREETYPE_H
// clang-format on

namespace sdl::text
{
    // This makes some things easier to read and type.
    using OptionalGlyph = std::optional<std::reference_wrapper<sdl::text::GlyphData>>;

    /// @brief This class serves to wrap FreeType and the System font.
    class SystemFont final
    {
        public:
            /// @brief Initializes Freetype and the system font.
            static bool initialize();

            /// @brief Exits the PL service and freetype.
            static void exit();

            /// @brief Resizes the font in freetype.
            static void resize(int fontSize) noexcept;

            /// @brief Attempts to find or load the codepoint passed.
            /// @param codepoint Codepoint.
            static sdl::text::OptionalGlyph find_load_glyph(uint32_t codepoint);

        private:
            /// @brief Freetype library.
            static inline FT_Library sm_ftLib{};

            /// @brief Faces of the system font.
            static inline std::array<FT_Face, PlSharedFontType_Total> sm_faces{};

            /// @brief This is the font size. 12 is the default.
            static inline int sm_fontSize = 12;

            /// @brief Mapped glyph cache. To do: Use an atlas instead when I have more time.
            static inline std::map<std::pair<uint32_t, int>, GlyphData> sm_glyphCache{};

            /// @brief Loads the codepoint from the system font using the currently set fontsize. Returns nullptr if the glyph
            /// isn't found.
            static FT_GlyphSlot load_glyph(uint32_t codepoint);

            /// @brief Uses the glyph slot passed to create a glyph texture.
            static sdl::SharedTexture convert_slot_to_texture(FT_GlyphSlot slot);
    };
}