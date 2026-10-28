#ifndef GTA2SC_ASIPLUGIN_FRONTENDDESTRUCTORHOOK_H
#define GTA2SC_ASIPLUGIN_FRONTENDDESTRUCTORHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueFrontendDestructorHook(std::span<const std::uint8_t> textSection);
}

#endif
