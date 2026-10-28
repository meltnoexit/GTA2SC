#include "KanjiExplicitColorHook.h"
#include "hook/util/PatternScanner.h"

#include <cstdint>
#include <MinHook.h>

#include "../util/MinHookErrorLogger.h"
#include "KanjiColor.h"
#include <string_view>

namespace GTA2SC {
    namespace {
        constexpr std::string_view kKanjiExplicitColorPattern =
            "8B 44 24 04 25 FF FF 00 00 83 E8 00 74 1E 83 E8 05 74 0E 48 75 1E C6 41 24 47 C6 41 25 43 C2 04";

        void (__thiscall *originalKanjiExplicitColor)(Kanji *, std::uint32_t) = nullptr;
        void *originalKanjiExplicitColorEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedKanjiExplicitColor(Kanji *kanji, void * /* edx */, std::uint32_t colorId) {
            if (!ApplyKanjiExplicitColor(kanji, colorId)) {
                originalKanjiExplicitColor(kanji, colorId);
            }
        }
    }

    bool QueueKanjiExplicitColorHook(std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalKanjiExplicitColorEntry =
            const_cast<std::uint8_t *>(FindPattern(textSection, kKanjiExplicitColorPattern));
        if (originalKanjiExplicitColorEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalKanjiExplicitColorEntry,
                                        reinterpret_cast<void *>(&HookedKanjiExplicitColor),
                                        reinterpret_cast<void **>(&originalKanjiExplicitColor));
        if (status != MH_OK) {
            LogMinHookError("KanjiExplicitColorHook: MH_CreateHook", status);
            originalKanjiExplicitColor = nullptr;
            originalKanjiExplicitColorEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalKanjiExplicitColorEntry);
        if (status != MH_OK) {
            LogMinHookError("KanjiExplicitColorHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalKanjiExplicitColorEntry);
            originalKanjiExplicitColor = nullptr;
            originalKanjiExplicitColorEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
