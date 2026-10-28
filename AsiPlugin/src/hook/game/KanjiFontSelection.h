#ifndef GTA2SC_ASIPLUGIN_KANJIFONTSELECTION_H
#define GTA2SC_ASIPLUGIN_KANJIFONTSELECTION_H

#include <cstdint>

namespace GTA2SC {
    struct TextFontSelection {
            std::uint32_t fontArgument{};
            std::uint16_t kanjiGlyphWidth = 0;
    };

    struct GlyphFontSelection {
            std::uint16_t fontId{};
            std::uint16_t kanjiGlyphWidth = 0;
    };

    TextFontSelection SelectTextFont(const std::uint16_t *text, std::uint32_t fontArgument);

    GlyphFontSelection SelectGlyphFont(std::uint16_t fontId, std::uint16_t character);
}

#endif
