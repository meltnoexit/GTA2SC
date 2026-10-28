#ifndef GTA2SC_ASIPLUGIN_KANJIOUTLINEHOOK_H
#define GTA2SC_ASIPLUGIN_KANJIOUTLINEHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueKanjiOutlineHook(std::span<const std::uint8_t> textSection);
}

#endif
