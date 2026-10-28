#ifndef GTA2SC_ASIPLUGIN_KANJIDEFAULTCOLORHOOK_H
#define GTA2SC_ASIPLUGIN_KANJIDEFAULTCOLORHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueKanjiDefaultColorHook(std::span<const std::uint8_t> textSection);
}

#endif
