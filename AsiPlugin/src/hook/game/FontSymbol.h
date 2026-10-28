#ifndef GTA2SC_ASIPLUGIN_FONTSYMBOLS_H
#define GTA2SC_ASIPLUGIN_FONTSYMBOLS_H

#include <cstdint>
#include <span>

#include "Kanji.h"

namespace GTA2SC {
    extern const std::uint16_t *smallFontId;
    extern const std::uint16_t *largeFontId;

    extern void **styleSlot;
    extern void **spriteManagerSlot;
    extern void(__thiscall *kanjiTextureBinding)(Kanji *);

    bool ResolveFontSymbol(std::span<const std::uint8_t> textSection);
}

#endif
