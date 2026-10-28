#include "PlayerNameDrawHook.h"

#include <MinHook.h>
#include <string_view>

#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiColor.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kPlayerNameDrawPattern = "A1 ?? ?? ?? ?? 83 EC 1C 85 C0 0F 84 ?? ?? ?? ?? A0 ?? ?? "
                                                            "?? ?? 84 C0 0F 84 ?? ?? ?? ?? 56 8B 35 ?? ?? ?? ??";

        void(__cdecl *originalPlayerNameDraw)() = nullptr;
        void *originalPlayerNameDrawEntry = nullptr;

        bool hookQueued = false;

        void __cdecl HookedPlayerNameDraw() {
            const KanjiDrawSourceScope scope(KanjiDrawSource::PlayerName);
            originalPlayerNameDraw();
        }
    }

    bool QueuePlayerNameDrawHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalPlayerNameDrawEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kPlayerNameDrawPattern));
        if (originalPlayerNameDrawEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalPlayerNameDrawEntry, reinterpret_cast<void *>(&HookedPlayerNameDraw),
                                         reinterpret_cast<void **>(&originalPlayerNameDraw));
        if (status != MH_OK) {
            LogMinHookError("PlayerNameDrawHook: MH_CreateHook", status);
            originalPlayerNameDraw = nullptr;
            originalPlayerNameDrawEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalPlayerNameDrawEntry);
        if (status != MH_OK) {
            LogMinHookError("PlayerNameDrawHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalPlayerNameDrawEntry);
            originalPlayerNameDraw = nullptr;
            originalPlayerNameDrawEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
