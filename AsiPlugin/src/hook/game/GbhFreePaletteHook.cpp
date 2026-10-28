#include "GbhFreePaletteHook.h"

#include <MinHook.h>

#include "KanjiRendererContext.h"
#include "hook/util/MinHookErrorLogger.h"

namespace GTA2SC {
    namespace {
        std::uintptr_t(WINAPI *originalGbhFreePalette)(std::uint32_t) = nullptr;
        void *originalGbhFreePaletteEntry = nullptr;
        bool hookQueued = false;

        std::uintptr_t WINAPI HookedGbhFreePalette(std::uint32_t palette) {
            ReleaseKanjiGlyphCache(kanjiRendererContext.freeTexture);
            return originalGbhFreePalette(palette);
        }
    }

    bool QueueGbhFreePaletteHook() {
        if (hookQueued) {
            return true;
        }

        if (kanjiRendererContext.freeTexture == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHookApiEx(L"d3ddll.dll", "gbh_FreePalette",
                                              reinterpret_cast<void *>(&HookedGbhFreePalette),
                                              reinterpret_cast<void **>(&originalGbhFreePalette),
                                              &originalGbhFreePaletteEntry);
        if (status != MH_OK) {
            LogMinHookError("GbhFreePaletteHook: MH_CreateHookApiEx", status);
            originalGbhFreePalette = nullptr;
            originalGbhFreePaletteEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalGbhFreePaletteEntry);
        if (status != MH_OK) {
            LogMinHookError("GbhFreePaletteHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalGbhFreePaletteEntry);
            originalGbhFreePalette = nullptr;
            originalGbhFreePaletteEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
