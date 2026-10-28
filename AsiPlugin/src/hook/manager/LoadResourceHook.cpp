#include "LoadResourceHook.h"

#include <MinHook.h>

#include "../util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"

namespace GTA2SC {
    namespace {
        constexpr DWORD kJapaneseLanguageId = 5;
        constexpr auto kLanguageAssignmentPattern =
            "8B F1 C7 05 ?? ?? ?? ?? 05 00 00 00 E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ??";

        constexpr WORD kOriginalDebugDialogId = 129;
        constexpr WORD kReplacementDebugDialogId = 1001;
        constexpr WORD kOriginalPhysicsDialogId = 140;
        constexpr WORD kReplacementPhysicsDialogId = 1002;

        const volatile DWORD *languageState = nullptr;

        HMODULE managerModule = nullptr;
        HMODULE pluginModule = nullptr;

        HRSRC originalDebugResource = nullptr;
        HRSRC replacementDebugResource = nullptr;
        HRSRC originalPhysicsResource = nullptr;
        HRSRC replacementPhysicsResource = nullptr;

        decltype(&::LoadResource) originalLoadResource = nullptr;
        void *originalLoadResourceEntry = nullptr;

        bool hookQueued = false;

        HGLOBAL WINAPI HookedLoadResource(HMODULE module, HRSRC resource) {
            if (module == managerModule && (resource == originalDebugResource || resource == originalPhysicsResource) &&
                *languageState == kJapaneseLanguageId) {
                if (resource == originalDebugResource) {
                    return originalLoadResource(pluginModule, replacementDebugResource);
                }
                if (resource == originalPhysicsResource) {
                    return originalLoadResource(pluginModule, replacementPhysicsResource);
                }
            }
            return originalLoadResource(module, resource);
        }
    }

    bool QueueLoadResourceHook(const HMODULE managerModuleHandle, const HMODULE pluginModuleHandle,
                               const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        if (managerModuleHandle == nullptr || pluginModuleHandle == nullptr) {
            return false;
        }

        const std::uintptr_t languageAddress = FindAbsoluteAddress32(textSection, kLanguageAssignmentPattern, 4);
        if (languageAddress == 0) {
            return false;
        }
        managerModule = managerModuleHandle;
        pluginModule = pluginModuleHandle;
        languageState = reinterpret_cast<const volatile DWORD *>(languageAddress);

        const auto dialogType = MAKEINTRESOURCEA(5);
        originalDebugResource =
            FindResourceA(managerModuleHandle, MAKEINTRESOURCEA(kOriginalDebugDialogId), dialogType);
        replacementDebugResource =
            FindResourceA(pluginModuleHandle, MAKEINTRESOURCEA(kReplacementDebugDialogId), dialogType);
        originalPhysicsResource =
            FindResourceA(managerModuleHandle, MAKEINTRESOURCEA(kOriginalPhysicsDialogId), dialogType);
        replacementPhysicsResource =
            FindResourceA(pluginModuleHandle, MAKEINTRESOURCEA(kReplacementPhysicsDialogId), dialogType);

        MH_STATUS status =
            MH_CreateHookApiEx(L"kernel32.dll", "LoadResource", reinterpret_cast<void *>(&HookedLoadResource),
                               reinterpret_cast<void **>(&originalLoadResource), &originalLoadResourceEntry);
        if (status != MH_OK) {
            LogMinHookError("LoadResourceHook:MH_CreateHookApiEx", status);
            originalLoadResource = nullptr;
            originalLoadResourceEntry = nullptr;
            languageState = nullptr;
            managerModule = nullptr;
            pluginModule = nullptr;
            originalDebugResource = nullptr;
            replacementDebugResource = nullptr;
            originalPhysicsResource = nullptr;
            replacementPhysicsResource = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalLoadResourceEntry);
        if (status != MH_OK) {
            LogMinHookError("LoadResourceHook:MH_QueueEnableHook", status);
            MH_RemoveHook(originalLoadResourceEntry);
            originalLoadResource = nullptr;
            originalLoadResourceEntry = nullptr;
            languageState = nullptr;
            managerModule = nullptr;
            pluginModule = nullptr;
            originalDebugResource = nullptr;
            replacementDebugResource = nullptr;
            originalPhysicsResource = nullptr;
            replacementPhysicsResource = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
