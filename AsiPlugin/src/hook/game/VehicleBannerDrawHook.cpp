#include "VehicleBannerDrawHook.h"

#include <MinHook.h>
#include <string_view>

#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "KanjiColor.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kVehicleBannerDrawPattern =
            "51 53 56 8B F1 57 80 3E 00 0F 84 ?? ?? ?? ?? 6A 0B E8 ?? ?? ?? ?? 8B 4E 44 8B F8 6A 00 BB 02 00 00 00";

        void(__thiscall *originalVehicleBannerDraw)(void *) = nullptr;
        void *originalVehicleBannerDrawEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedVehicleBannerDraw(void *banner, void * /* edx */) {
            const KanjiDrawSourceScope scope(KanjiDrawSource::VehicleBanner);
            originalVehicleBannerDraw(banner);
        }
    }

    bool QueueVehicleBannerDrawHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalVehicleBannerDrawEntry =
            const_cast<std::uint8_t *>(FindPattern(textSection, kVehicleBannerDrawPattern));
        if (originalVehicleBannerDrawEntry == nullptr) {
            return false;
        }

        MH_STATUS status =
            MH_CreateHook(originalVehicleBannerDrawEntry, reinterpret_cast<void *>(&HookedVehicleBannerDraw),
                          reinterpret_cast<void **>(&originalVehicleBannerDraw));
        if (status != MH_OK) {
            LogMinHookError("VehicleBannerDrawHook: MH_CreateHook", status);
            originalVehicleBannerDraw = nullptr;
            originalVehicleBannerDrawEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalVehicleBannerDrawEntry);
        if (status != MH_OK) {
            LogMinHookError("VehicleBannerDrawHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalVehicleBannerDrawEntry);
            originalVehicleBannerDraw = nullptr;
            originalVehicleBannerDrawEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
