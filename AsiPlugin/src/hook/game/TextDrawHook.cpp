#include "TextDrawHook.h"

#include <cstdint>
#include <MinHook.h>
#include <string_view>

#include "../util/MinHookErrorLogger.h"
#include "FontSymbol.h"
#include "hook/util/PatternScanner.h"
#include "KanjiColor.h"
#include "KanjiFontSelection.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kTextDrawPattern =
            "83 EC 44 8A 44 24 68 8B 4C 24 64 53 55 56 57 E8 ?? ?? ?? ?? 8B 7C 24 58 8B F0 8B 44 24 5C 8D 4C";

        void(__stdcall *originalTextDraw)(const std::uint16_t *, std::uintptr_t, std::uintptr_t, std::uint32_t,
                                          std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t,
                                          std::uintptr_t) = nullptr;
        void *originalTextDrawEntry = nullptr;

        bool hookQueued = false;

        void __stdcall HookedTextDraw(const std::uint16_t *text, std::uintptr_t a2, std::uintptr_t a3,
                                      std::uint32_t fontArgument, std::uintptr_t a5, std::uintptr_t a6,
                                      std::uintptr_t a7, std::uintptr_t a8, std::uintptr_t a9) {
            const KanjiColorScope colorScope(static_cast<std::uint16_t>(fontArgument), *smallFontId);
            const TextFontSelection selection = SelectTextFont(text, fontArgument);

            originalTextDraw(text, a2, a3, selection.fontArgument, a5, a6, a7, a8, a9);
        }
    }

    bool QueueTextDrawHook(const HMODULE gameModuleHandle, const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        if (gameModuleHandle == nullptr) {
            return false;
        }

        originalTextDrawEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kTextDrawPattern));
        if (originalTextDrawEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalTextDrawEntry, reinterpret_cast<void *>(&HookedTextDraw),
                                         reinterpret_cast<void **>(&originalTextDraw));
        if (status != MH_OK) {
            LogMinHookError("TextDrawHook: MH_CreateHook", status);
            originalTextDraw = nullptr;
            originalTextDrawEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalTextDrawEntry);
        if (status != MH_OK) {
            LogMinHookError("TextDrawHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalTextDrawEntry);
            originalTextDraw = nullptr;
            originalTextDrawEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
