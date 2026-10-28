#ifndef GTA2SC_ASIPLUGIN_GAMECONSTRUCTORHOOK_H
#define GTA2SC_ASIPLUGIN_GAMECONSTRUCTORHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueGameConstructorHook(std::span<const std::uint8_t> textSection);
}

#endif
