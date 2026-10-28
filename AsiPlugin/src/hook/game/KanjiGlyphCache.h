#ifndef GTA2SC_ASIPLUGIN_KANJIGLYPHCACHE_H
#define GTA2SC_ASIPLUGIN_KANJIGLYPHCACHE_H

#include <Windows.h>

#include "GbhTexture.h"

namespace GTA2SC {
    using RegisterGbhTextureFn = GbhTexture *(WINAPI *)(std::uint16_t, std::uint16_t, const void *, std::uint32_t, std::uint8_t);
    using FreeGbhTextureFn = std::uintptr_t(WINAPI *)(GbhTexture *);

    GbhTexture *GetCachedKanjiTexture(GbhTexture *source, std::uint32_t drawFlags,
                                    RegisterGbhTextureFn registerTexture, FreeGbhTextureFn freeTexture);
    void ReleaseKanjiGlyphCache(FreeGbhTextureFn freeTexture, const GbhTexture *owner = nullptr);
}

#endif
