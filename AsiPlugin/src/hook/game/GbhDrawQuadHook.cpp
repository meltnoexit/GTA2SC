#include "GbhDrawQuadHook.h"

#include <MinHook.h>

#include "FrontendKanjiState.h"
#include "KanjiRendererContext.h"
#include "hook/util/MinHookErrorLogger.h"

namespace GTA2SC {
    namespace {
        std::uintptr_t(WINAPI *originalGbhDrawQuad)(std::uint32_t, GbhTexture *, const void *, std::uint32_t) = nullptr;
        void *originalGbhDrawQuadEntry = nullptr;

        bool hookQueued = false;

        std::uintptr_t WINAPI HookedGbhDrawQuad(std::uint32_t flags, GbhTexture *texture, const void *vertices,
                                           std::uint32_t color) {
            const auto *kanji = activeKanjiSlot == nullptr ? nullptr : *activeKanjiSlot;
            if (texture != nullptr && kanji != nullptr &&
                (texture == kanji->smallTexture || texture == kanji->largeTexture)) {
                texture = GetCachedKanjiTexture(texture, flags, kanjiRendererContext.registerTexture, kanjiRendererContext.freeTexture);
            }
            return originalGbhDrawQuad(flags, texture, vertices, color);
        }
    }

    bool QueueGbhDrawQuadHook() {
        if (hookQueued) {
            return true;
        }

        if (kanjiRendererContext.freeTexture == nullptr) {
            return false;
        }

        kanjiRendererContext.registerTexture = reinterpret_cast<RegisterGbhTextureFn>(
            GetProcAddress(GetModuleHandleW(L"d3ddll.dll"), "gbh_RegisterTexture"));
        if (kanjiRendererContext.registerTexture == nullptr) {
            OutputDebugStringA("GTA2SC: gbh_RegisterTexture export not found.\n");
            return false;
        }

        MH_STATUS status = MH_CreateHookApiEx(L"d3ddll.dll", "gbh_DrawQuad",
                                              reinterpret_cast<void *>(&HookedGbhDrawQuad),
                                              reinterpret_cast<void **>(&originalGbhDrawQuad),
                                              &originalGbhDrawQuadEntry);
        if (status != MH_OK) {
            LogMinHookError("GbhDrawQuadHook: MH_CreateHookApiEx", status);
            originalGbhDrawQuad = nullptr;
            originalGbhDrawQuadEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalGbhDrawQuadEntry);
        if (status != MH_OK) {
            LogMinHookError("GbhDrawQuadHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalGbhDrawQuadEntry);
            originalGbhDrawQuad = nullptr;
            originalGbhDrawQuadEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
