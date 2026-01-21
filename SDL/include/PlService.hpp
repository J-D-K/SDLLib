#pragma once
#include "CoreComponent.hpp"

#include <array>
#include <switch.h>

namespace sdl2
{
    /// @brief Forward declaration.
    class SystemFont;

    /// @brief Wrapper class around Pl.
    class PlService final : public sdl2::CoreComponent
    {
        public:
            /// @brief Constructor. Initializes PL and pulls font data from the system.
            PlService()
            {
                const Result plInit = plInitialize(PlServiceType_User);
                if (R_FAILED(plInit)) { return; }

                // Get the shared font.
                int32_t fontTotal{};
                const uint64_t languageCode = PlService::get_language_code();
                const Result getShared =
                    plGetSharedFont(languageCode, m_sharedFonts.data(), PlSharedFontType_Total, &fontTotal);
                if (R_FAILED(getShared)) { return; }

                m_isInitialized = true;
            }

            /// @brief Exits the pl service.
            ~PlService() { plExit(); }

            /// @brief So System font has easier access to what it needs.
            friend class SystemFont;

        private:
            /// @brief Array of shared font data.
            std::array<PlFontData, PlSharedFontType_Total> m_sharedFonts{};

            /// @brief Gets the language code from the system.
            uint64_t get_language_code()
            {
                // Init set.
                const Result setInit = setInitialize();
                if (R_FAILED(setInit)) { return SetLanguage_ENUS; }

                // Grab language code.
                uint64_t languageCode{};
                const Result getLanguageCode = setGetLanguageCode(&languageCode);
                if (R_FAILED(getLanguageCode)) { languageCode = SetLanguage_ENUS; }

                // Exit set?
                setExit();

                // Return code.
                return languageCode;
            };
    };
}