#ifndef GTA2SC_ASIPLUGIN_ZONEBANNERDRAWHOOK_H
#define GTA2SC_ASIPLUGIN_ZONEBANNERDRAWHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueZoneBannerDrawHook(std::span<const std::uint8_t> textSection);
}

#endif
