#ifndef GTA2SC_ASIPLUGIN_VEHICLEBANNERDRAWHOOK_H
#define GTA2SC_ASIPLUGIN_VEHICLEBANNERDRAWHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueVehicleBannerDrawHook(std::span<const std::uint8_t> textSection);
}

#endif
