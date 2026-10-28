#ifndef GTA2SC_ASIPLUGIN_KANJIEXPLICITCOLORHOOK_H
#define GTA2SC_ASIPLUGIN_KANJIEXPLICITCOLORHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueKanjiExplicitColorHook(std::span<const std::uint8_t> textSection);
}

#endif
