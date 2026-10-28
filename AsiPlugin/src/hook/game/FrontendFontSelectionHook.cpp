#include "FrontendFontSelectionHook.h"

#include <cstdint>
#include <MinHook.h>
#include <string_view>

#include "FontSymbol.h"
#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "FrontendKanjiState.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kFrontendFontSelectionPattern =
            "56 8B F1 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 74 0F 66 A1 ?? ?? ?? ?? 66 89 86 1C 01 00 00 5E";

        constexpr std::size_t kFrontendFontIdOffset = 0x11C;

        void(__thiscall *originalFrontendFontSelection)(void *) = nullptr;
        void *originalFrontendFontSelectionEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedFrontendFontSelection(void *frontend, void * /* edx */) {
            originalFrontendFontSelection(frontend);

            auto *selectedFontId =
                reinterpret_cast<std::uint16_t *>(static_cast<std::uint8_t *>(frontend) + kFrontendFontIdOffset);
            if (*selectedFontId != *smallFontId) {
                return;
            }

            InitializeFrontendKanji();
        }
    }

    bool QueueFrontendFontSelectionHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalFrontendFontSelectionEntry =
            const_cast<std::uint8_t *>(FindPattern(textSection, kFrontendFontSelectionPattern));
        if (originalFrontendFontSelectionEntry == nullptr) {
            return false;
        }

        MH_STATUS mh_status =
            MH_CreateHook(originalFrontendFontSelectionEntry, reinterpret_cast<void *>(&HookedFrontendFontSelection),
                          reinterpret_cast<void **>(&originalFrontendFontSelection));
        if (mh_status != MH_OK) {
            LogMinHookError("FrontendFontSelectionHook:MH_CreateHook()", mh_status);
            originalFrontendFontSelection = nullptr;
            originalFrontendFontSelectionEntry = nullptr;
            return false;
        }

        mh_status = MH_QueueEnableHook(originalFrontendFontSelectionEntry);
        if (mh_status != MH_OK) {
            LogMinHookError("FrontendFontSelectionHook:MH_QueueEnableHook()", mh_status);
            MH_RemoveHook(originalFrontendFontSelectionEntry);
            originalFrontendFontSelection = nullptr;
            originalFrontendFontSelectionEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
