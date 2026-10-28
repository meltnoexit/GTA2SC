#include "GlyphWidthHook.h"

#include <cstdint>
#include <MinHook.h>
#include <string_view>

#include "../util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiFontSelection.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kGlyphWidthPattern =
            "8B 44 24 04 66 8B 00 66 3B 05 ?? ?? ?? ?? 75 07 66 B8 10 00 C2 08 00 66 3B 05 ?? ?? ?? ?? 75 07";

        std::uint16_t(__thiscall *originalGlyphWidth)(void *style, const std::uint16_t *fontId,
                                                      const std::uint16_t *character) = nullptr;
        void *originalGlyphWidthEntry = nullptr;

        bool hookQueued = false;

        std::uint16_t __fastcall HookedGlyphWidth(void *style, void * /* edx */, const std::uint16_t *fontId,
                                                  const std::uint16_t *character) {
            if (fontId == nullptr || character == nullptr) {
                return originalGlyphWidth(style, fontId, character);
            }

            const auto selection = SelectGlyphFont(*fontId, *character);
            if (selection.kanjiGlyphWidth != 0) {
                return selection.kanjiGlyphWidth;
            }

            return originalGlyphWidth(style, &selection.fontId, character);
        }
    }

    bool QueueGlyphWidthHook(const HMODULE gameModuleHandle, const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        if (gameModuleHandle == nullptr) {
            return false;
        }

        originalGlyphWidthEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kGlyphWidthPattern));
        if (originalGlyphWidthEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalGlyphWidthEntry, reinterpret_cast<void *>(&HookedGlyphWidth),
                                         reinterpret_cast<void **>(&originalGlyphWidth));
        if (status != MH_OK) {
            LogMinHookError("GlyphWidthHook: MH_CreateHook", status);
            originalGlyphWidth = nullptr;
            originalGlyphWidthEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalGlyphWidthEntry);
        if (status != MH_OK) {
            LogMinHookError("GlyphWidthHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalGlyphWidthEntry);
            originalGlyphWidth = nullptr;
            originalGlyphWidthEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
