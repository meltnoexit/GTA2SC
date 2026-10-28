#include "TextMeasureHook.h"


#include <cstdint>
#include <string_view>
#include <MinHook.h>

#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiFontSelection.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kTextMeasurePattern =
            "8B 0D ?? ?? ?? ?? 53 55 56 8D 44 24 14 57 8B 7C 24 14 50 33 F6 E8 ?? ?? ?? ?? 66 8B D8 33 ED 81";

        int(__stdcall *originalTextMeasure)(const std::uint16_t *text, std::uint32_t fontArgument) = nullptr;
        void *originalTextMeasureEntry = nullptr;

        bool hookQueued = false;

        int __stdcall HookedTextMeasure(const std::uint16_t *text, std::uint32_t fontArgument) {
            const TextFontSelection selection = SelectTextFont(text, fontArgument);
            const auto width = selection.kanjiGlyphWidth;
            if (width == 0) {
                return originalTextMeasure(text, selection.fontArgument);
            }

            int lineWidth = 0;
            int maximumWidth = 0;
            for (const auto *character = text; *character != 0; ++character) {
                if (*character == '\n') {
                    if (lineWidth > maximumWidth) {
                        maximumWidth = lineWidth;
                    }
                    lineWidth = 0;
                } else if (*character != '#') {
                    lineWidth += width;
                }
            }
            return lineWidth > maximumWidth ? lineWidth : maximumWidth;
        }
    }

    bool QueueTextMeasureHook(const HMODULE gameModuleHandle,
                              std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }
        if (gameModuleHandle == nullptr) {
            return false;
        }

        originalTextMeasureEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kTextMeasurePattern));
        if (originalTextMeasureEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalTextMeasureEntry, reinterpret_cast<void *>(&HookedTextMeasure),
                                         reinterpret_cast<void **>(&originalTextMeasure));

        if (status != MH_OK) {
            LogMinHookError("TextMeasureHook: MH_CreateHook", status);
            originalTextMeasure = nullptr;
            originalTextMeasureEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalTextMeasureEntry);
        if (status != MH_OK) {
            LogMinHookError("TextMeasureHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalTextMeasureEntry);
            originalTextMeasure = nullptr;
            originalTextMeasureEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
