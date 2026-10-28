#ifndef GTA2SC_ASIPLUGIN_GLYPHWIDTHHOOK_H
#define GTA2SC_ASIPLUGIN_GLYPHWIDTHHOOK_H

#include <Windows.h>

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueGlyphWidthHook(HMODULE gameModuleHandle, std::span<const std::uint8_t> textSection);
}

#endif
