#ifndef GTA2SC_ASIPLUGIN_FRONTENDKANJISTATE_H
#define GTA2SC_ASIPLUGIN_FRONTENDKANJISTATE_H

#include <Windows.h>

#include <cstdint>
#include <span>

#include "Kanji.h"

namespace GTA2SC {
    extern Kanji **activeKanjiSlot;

    bool ResolveFrontendKanjiSymbol(std::span<const std::uint8_t> textSection);
    bool InitializeFrontendKanji();
    void ReleaseFrontendKanji();
}

#endif
