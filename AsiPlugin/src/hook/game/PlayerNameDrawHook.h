#ifndef GTA2SC_ASIPLUGIN_PLAYERNAMEDRAWHOOK_H
#define GTA2SC_ASIPLUGIN_PLAYERNAMEDRAWHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueuePlayerNameDrawHook(std::span<const std::uint8_t> textSection);
}

#endif
