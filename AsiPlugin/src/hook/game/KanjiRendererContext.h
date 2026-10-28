#ifndef GTA2SC_ASIPLUGIN_KANJIRENDERERCONTEXT_H
#define GTA2SC_ASIPLUGIN_KANJIRENDERERCONTEXT_H

#include "KanjiGlyphCache.h"

namespace GTA2SC {
    struct KanjiRendererContext {
        RegisterGbhTextureFn registerTexture = nullptr;
        FreeGbhTextureFn freeTexture = nullptr;
    };

    extern KanjiRendererContext kanjiRendererContext;
}

#endif
