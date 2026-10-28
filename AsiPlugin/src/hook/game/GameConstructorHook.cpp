#include "GameConstructorHook.h"
#include "hook/util/PatternScanner.h"

#include <cstdint>
#include <MinHook.h>

#include "../util/MinHookErrorLogger.h"
#include "FrontendKanjiState.h"
#include <string_view>

namespace GTA2SC {
    namespace {
        constexpr std::string_view kGameConstructorPattern =
            "6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 64 89 25 00 00 00 00 51 53 55 56 8B F1 57 B9 ?? ?? ?? ??";

        void *(__thiscall *originalGameConstructor)(void *game, char a3, std::uint8_t a4) = nullptr;
        void *originalGameConstructorEntry = nullptr;

        bool hookQueued = false;

        void *__fastcall HookedGameConstructor(void *game, void * /* edx */, char a3, std::uint8_t a4) {
            ReleaseFrontendKanji();
            return originalGameConstructor(game, a3, a4);
        }
    }

    bool QueueGameConstructorHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalGameConstructorEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kGameConstructorPattern));
        if (originalGameConstructorEntry == nullptr) {
            return false;
        }

        MH_STATUS mh_status =
            MH_CreateHook(originalGameConstructorEntry, reinterpret_cast<void *>(&HookedGameConstructor),
                          reinterpret_cast<void **>(&originalGameConstructor));
        if (mh_status != MH_OK) {
            LogMinHookError("GameConstructorHook:MH_CreateHook()", mh_status);
            originalGameConstructor = nullptr;
            originalGameConstructorEntry = nullptr;
            return false;
        }

        mh_status = MH_QueueEnableHook(originalGameConstructorEntry);
        if (mh_status != MH_OK) {
            LogMinHookError("GameConstructorHook:MH_QueueEnableHook()", mh_status);
            MH_RemoveHook(originalGameConstructorEntry);
            originalGameConstructor = nullptr;
            originalGameConstructorEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
