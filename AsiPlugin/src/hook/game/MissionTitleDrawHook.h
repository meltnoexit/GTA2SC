#ifndef GTA2SC_ASIPLUGIN_MISSIONTITLEDRAWHOOK_H
#define GTA2SC_ASIPLUGIN_MISSIONTITLEDRAWHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueMissionTitleDrawHook(std::span<const std::uint8_t> textSection);
}

#endif
