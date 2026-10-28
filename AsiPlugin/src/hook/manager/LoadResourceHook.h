#ifndef GTA2SC_ASIPLUGIN_LOADRESOURCEHOOK_H
#define GTA2SC_ASIPLUGIN_LOADRESOURCEHOOK_H

#include <Windows.h>

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueLoadResourceHook(HMODULE managerModuleHandle, HMODULE pluginModuleHandle,
                               std::span<const std::uint8_t> textSection);
}

#endif
