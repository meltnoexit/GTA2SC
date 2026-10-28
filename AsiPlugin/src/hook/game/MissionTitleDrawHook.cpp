#include "MissionTitleDrawHook.h"

#include <MinHook.h>
#include <string_view>

#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiColor.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kMissionTitleDrawPattern =
            "51 56 8B F1 80 3E 00 74 ?? 66 8B 0D ?? ?? ?? ?? 8B 96 C0 01 00 00 6A 00 6A 00 8D 44 24 0C 6A 00";

        void(__thiscall *originalMissionTitleDraw)(void *) = nullptr;
        void *originalMissionTitleDrawEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedMissionTitleDraw(void *missionTitle, void * /* edx */) {
            const KanjiDrawSourceScope scope(KanjiDrawSource::MissionTitle);
            originalMissionTitleDraw(missionTitle);
        }
    }

    bool QueueMissionTitleDrawHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalMissionTitleDrawEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kMissionTitleDrawPattern));
        if (originalMissionTitleDrawEntry == nullptr) {
            return false;
        }

        MH_STATUS status =
            MH_CreateHook(originalMissionTitleDrawEntry, reinterpret_cast<void *>(&HookedMissionTitleDraw),
                          reinterpret_cast<void **>(&originalMissionTitleDraw));
        if (status != MH_OK) {
            LogMinHookError("MissionTitleDrawHook: MH_CreateHook", status);
            originalMissionTitleDraw = nullptr;
            originalMissionTitleDrawEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalMissionTitleDrawEntry);
        if (status != MH_OK) {
            LogMinHookError("MissionTitleDrawHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalMissionTitleDrawEntry);
            originalMissionTitleDraw = nullptr;
            originalMissionTitleDrawEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
