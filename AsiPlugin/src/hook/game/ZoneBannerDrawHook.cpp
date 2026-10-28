#include "ZoneBannerDrawHook.h"

#include <MinHook.h>

#include "../util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiColor.h"
#include <string_view>

namespace GTA2SC {
    namespace {
        constexpr std::string_view kZoneBannerDrawPattern =
            "51 53 55 56 8B F1 57 80 3E 00 0F 84 ?? ?? ?? ?? 68 9F 00 00 00 E8 ?? ?? ?? ?? 8B D8 8B 8E 90 00 00 00";

        void (__thiscall *originalZoneBannerDraw)(void *) = nullptr;
        void *originalZoneBannerDrawEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedZoneBannerDraw(void *banner, void * /* edx */) {
            const KanjiDrawSourceScope scope(KanjiDrawSource::ZoneBanner);
            originalZoneBannerDraw(banner);
        }
    }

    bool QueueZoneBannerDrawHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalZoneBannerDrawEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kZoneBannerDrawPattern));
        if (originalZoneBannerDrawEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalZoneBannerDrawEntry, reinterpret_cast<void *>(&HookedZoneBannerDraw),
                                         reinterpret_cast<void **>(&originalZoneBannerDraw));
        if (status != MH_OK) {
            LogMinHookError("ZoneBannerDrawHook: MH_CreateHook", status);
            originalZoneBannerDraw = nullptr;
            originalZoneBannerDrawEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalZoneBannerDrawEntry);
        if (status != MH_OK) {
            LogMinHookError("ZoneBannerDrawHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalZoneBannerDrawEntry);
            originalZoneBannerDraw = nullptr;
            originalZoneBannerDrawEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
