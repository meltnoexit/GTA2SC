#ifndef GTA2SC_ASIPLUGIN_TEXTDRAWHOOK_H
#define GTA2SC_ASIPLUGIN_TEXTDRAWHOOK_H

#include <Windows.h>

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueTextDrawHook(HMODULE gameModuleHandle, std::span<const std::uint8_t> textSection);
}

#endif
