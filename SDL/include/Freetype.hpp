#pragma once
#include "CoreComponent.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace sdl2
{
    /// @brief Forward declaration to allow the Font class to access the FT_Library.
    class Font;

    /// @brief Wrapper class around Freetype.
    class Freetype final : public sdl2::CoreComponent
    {
        public:
            // No copying or moving.
            Freetype(const Freetype &)            = delete;
            Freetype(Freetype &&)                 = delete;
            Freetype &operator=(const Freetype &) = delete;
            Freetype &operator=(Freetype &&)      = delete;

            /// @brief Constructor. Initializes library.
            Freetype()
            {
                if (m_library) { return; }

                // Init library.
                const FT_Error ftError = FT_Init_FreeType(&m_library);
                if (ftError != 0) { return; }

                m_isInitialized = true;
            }

            /// @brief Destructor. Quits freetype.
            ~Freetype()
            {
                if (!m_library) { return; }

                // Quit freetype.
                FT_Done_FreeType(m_library);

                // This is just to be 100% sure.
                m_library = nullptr;
            }

            /// @brief This allows the Font class to use the library.
            friend class Font;

        private:
            /// @brief Library.
            FT_Library m_library{};
    };
}