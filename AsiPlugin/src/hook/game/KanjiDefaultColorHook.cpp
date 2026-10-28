#include "KanjiDefaultColorHook.h"

#include <MinHook.h>
#include <string_view>

#include "../util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiColor.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kKanjiDefaultColorPattern =
            "C6 41 24 FF C6 41 25 F1 C3 90 90 90 90 90 90 90 55 8B EC 83 E4 F8 83 EC";

        void(__thiscall *originalKanjiDefaultColor)(Kanji *) = nullptr;
        void *originalKanjiDefaultColorEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedKanjiDefaultColor(Kanji *kanji, void * /* edx */) {
            if (!ApplyKanjiDefaultColor(kanji)) {
                originalKanjiDefaultColor(kanji);
            }
        }
    }

    bool QueueKanjiDefaultColorHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalKanjiDefaultColorEntry =
            const_cast<std::uint8_t *>(FindPattern(textSection, kKanjiDefaultColorPattern));
        if (originalKanjiDefaultColorEntry == nullptr) {
            return false;
        }

        MH_STATUS status =
            MH_CreateHook(originalKanjiDefaultColorEntry, reinterpret_cast<void *>(&HookedKanjiDefaultColor),
                          reinterpret_cast<void **>(&originalKanjiDefaultColor));
        if (status != MH_OK) {
            LogMinHookError("KanjiDefaultColorHook: MH_CreateHook", status);
            originalKanjiDefaultColor = nullptr;
            originalKanjiDefaultColorEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalKanjiDefaultColorEntry);
        if (status != MH_OK) {
            LogMinHookError("KanjiDefaultColorHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalKanjiDefaultColorEntry);
            originalKanjiDefaultColor = nullptr;
            originalKanjiDefaultColorEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
