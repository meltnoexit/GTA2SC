#include <Windows.h>

#include <cstdint>
#include <MinHook.h>

#include "hook/util/MinHookErrorLogger.h"
#include "MilesAilStartupHook.h"

namespace GTA2SC {
    namespace {
        constexpr std::uint32_t kLockProtectionPreference = 18;

        std::int32_t(WINAPI *originalMilesAilStartup)() = nullptr;
        std::int32_t(WINAPI *MilesAilSetPreference)(std::uint32_t, std::int32_t) = nullptr;
        void *originalMilesAilStartupEntry = nullptr;

        bool hookQueued = false;

        std::int32_t WINAPI HookedMilesStartup() {
            const auto result = originalMilesAilStartup();
            MilesAilSetPreference(kLockProtectionPreference, 0);
            return result;
        }
    }

    bool QueueMilesStartupHook() {
        if (hookQueued) {
            return true;
        }

        const HMODULE milesModule = GetModuleHandleW(L"mss32.dll");
        if (milesModule == nullptr) {
            return false;
        }

        MilesAilSetPreference = reinterpret_cast<decltype(MilesAilSetPreference)>(
            GetProcAddress(milesModule, "_AIL_set_preference@8"));
        if (MilesAilSetPreference == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHookApiEx(L"mss32.dll", "_AIL_startup@0",
                                              reinterpret_cast<void *>(&HookedMilesStartup),
                                              reinterpret_cast<void **>(&originalMilesAilStartup),
                                              &originalMilesAilStartupEntry);
        if (status != MH_OK) {
            LogMinHookError("MilesAilStartupHook:MH_CreateHookApiEx", status);
            originalMilesAilStartup = nullptr;
            originalMilesAilStartupEntry = nullptr;
            MilesAilSetPreference = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalMilesAilStartupEntry);
        if (status != MH_OK) {
            LogMinHookError("MilesAilStartupHook:MH_QueueEnableHook", status);
            MH_RemoveHook(originalMilesAilStartupEntry);
            originalMilesAilStartup = nullptr;
            originalMilesAilStartupEntry = nullptr;
            MilesAilSetPreference = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
