#include "FrontendDestructorHook.h"

#include <MinHook.h>
#include <string_view>

#include "FrontendKanjiState.h"
#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kFrontendDestructorPattern =
            "6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 64 89 25 00 00 00 00 51 53 56 8B F1 57 89 74 24 0C C7";

        void(__thiscall *originalFrontendDestructor)(void *frontend) = nullptr;
        void *originalFrontendDestructorEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedFrontendDestructor(void *frontend, void * /* edx */) {
            ReleaseFrontendKanji();
            originalFrontendDestructor(frontend);
        }
    }

    bool QueueFrontendDestructorHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalFrontendDestructorEntry =
            const_cast<std::uint8_t *>(FindPattern(textSection, kFrontendDestructorPattern));
        if (originalFrontendDestructorEntry == nullptr) {
            return false;
        }

        MH_STATUS mh_status =
            MH_CreateHook(originalFrontendDestructorEntry, reinterpret_cast<void *>(&HookedFrontendDestructor),
                          reinterpret_cast<void **>(&originalFrontendDestructor));
        if (mh_status != MH_OK) {
            LogMinHookError("MH_CreateHook", mh_status);
            originalFrontendDestructor = nullptr;
            originalFrontendDestructorEntry = nullptr;
            return false;
        }

        mh_status = MH_QueueEnableHook(originalFrontendDestructorEntry);
        if (mh_status != MH_OK) {
            LogMinHookError("MH_QueueEnableHook", mh_status);
            MH_RemoveHook(originalFrontendDestructorEntry);
            originalFrontendDestructor = nullptr;
            originalFrontendDestructorEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
