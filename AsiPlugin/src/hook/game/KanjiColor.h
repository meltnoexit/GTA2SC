#ifndef GTA2SC_ASIPLUGIN_KANJICOLOR_H
#define GTA2SC_ASIPLUGIN_KANJICOLOR_H

#include <array>
#include <cstdint>

#include "Kanji.h"

namespace GTA2SC {
    struct KanjiPaletteColor {
            std::uint8_t foreground;
            std::uint8_t edge = 0;
    };

    struct KanjiColorProfile {
            KanjiPaletteColor normal;
            const std::array<KanjiPaletteColor, 16> *remaps;
            KanjiPaletteColor fallback;
    };

    enum class KanjiDrawSource : std::uint8_t {
        MissionTitle = 4,
        PlayerName = 8,
        VehicleBanner = 1,
        ZoneBanner = 2
    };

    class KanjiDrawSourceScope {
        public:
            explicit KanjiDrawSourceScope(KanjiDrawSource source) noexcept;
            ~KanjiDrawSourceScope();

            KanjiDrawSourceScope(const KanjiDrawSourceScope &) = delete;
            KanjiDrawSourceScope &operator=(const KanjiDrawSourceScope &) = delete;

        private:
            std::uint8_t previous_;
    };

    class KanjiColorScope {
        public:
            KanjiColorScope(std::uint16_t fontId, std::uint16_t smallKanjiFontId) noexcept;
            ~KanjiColorScope();

            KanjiColorScope(const KanjiColorScope &) = delete;
            KanjiColorScope &operator=(const KanjiColorScope &) = delete;

        private:
            KanjiColorProfile profile_;
            const KanjiColorProfile *previous_;
    };

    bool ApplyKanjiDefaultColor(Kanji *kanji) noexcept;
    bool ApplyKanjiExplicitColor(Kanji *kanji, std::uint32_t colorId) noexcept;
}

#endif
