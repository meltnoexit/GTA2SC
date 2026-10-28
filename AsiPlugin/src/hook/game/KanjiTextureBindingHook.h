#ifndef GTA2SC_ASIPLUGIN_KANJITEXTUREBINDINGHOOK_H
#define GTA2SC_ASIPLUGIN_KANJITEXTUREBINDINGHOOK_H

#include <cstdint>
#include <span>

namespace GTA2SC {
    bool QueueKanjiTextureBindingHook(std::span<const std::uint8_t> textSection);
}

#endif
