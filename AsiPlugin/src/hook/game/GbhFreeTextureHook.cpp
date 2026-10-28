#include "GbhFreeTextureHook.h"

#include <MinHook.h>

#include "KanjiRendererContext.h"
#include "hook/util/MinHookErrorLogger.h"

namespace GTA2SC {
    namespace {
        FreeGbhTextureFn originalGbhFreeTexture = nullptr;
        void *originalGbhFreeTextureEntry = nullptr;

        bool hookQueued = false;

        std::uintptr_t WINAPI HookedGbhFreeTexture(GbhTexture *texture) {
            ReleaseKanjiGlyphCache(originalGbhFreeTexture, texture);
            return originalGbhFreeTexture(texture);
        }
    }

    bool QueueGbhFreeTextureHook() {
        if (hookQueued) {
            return true;
        }

        MH_STATUS status = MH_CreateHookApiEx(L"d3ddll.dll", "gbh_FreeTexture",
                                              reinterpret_cast<void *>(&HookedGbhFreeTexture),
                                              reinterpret_cast<void **>(&originalGbhFreeTexture),
                                              &originalGbhFreeTextureEntry);
        if (status != MH_OK) {
            LogMinHookError("GbhFreeTextureHook: MH_CreateHookApiEx", status);
            originalGbhFreeTexture = nullptr;
            originalGbhFreeTextureEntry = nullptr;
            return false;
        }
        kanjiRendererContext.freeTexture = originalGbhFreeTexture;

        status = MH_QueueEnableHook(originalGbhFreeTextureEntry);
        if (status != MH_OK) {
            LogMinHookError("GbhFreeTextureHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalGbhFreeTextureEntry);
            originalGbhFreeTexture = nullptr;
            originalGbhFreeTextureEntry = nullptr;
            kanjiRendererContext.freeTexture = nullptr;
            return false;
        }
        hookQueued = true;
        return true;
    }
}
