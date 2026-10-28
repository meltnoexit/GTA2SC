#ifndef GTA2SC_ASIPLUGIN_FRONTENDFONTSELECTIONHOOK_H
#define GTA2SC_ASIPLUGIN_FRONTENDFONTSELECTIONHOOK_H

#include <windows.h>

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueFrontendFontSelectionHook(std::span<const std::uint8_t> textSection);
}

#endif
