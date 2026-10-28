#include "LoadBitmapHook.h"

#include <MinHook.h>

#include "../util/MinHookErrorLogger.h"

namespace GTA2SC {
    namespace {
        constexpr WORD kOriginalBitmapId = 159;
        constexpr WORD kReplacementBitmapId = 1000;

        HMODULE managerModule = nullptr;
        HMODULE pluginModule = nullptr;

        decltype(&::LoadBitmapA) originalLoadBitmapA = nullptr;
        void *originalLoadBitmapEntry = nullptr;

        bool hookQueued = false;

        HBITMAP WINAPI HookedLoadBitmapA(HINSTANCE instance, LPCSTR bitmapName) {
            if (instance == managerModule && IS_INTRESOURCE(bitmapName) &&
                bitmapName == MAKEINTRESOURCEA(kOriginalBitmapId)) {
                HBITMAP replacementBitmap = originalLoadBitmapA(pluginModule, MAKEINTRESOURCEA(kReplacementBitmapId));
                if (replacementBitmap != nullptr) {
                    return replacementBitmap;
                }
            }
            return originalLoadBitmapA(instance, bitmapName);
        }
    }

    bool QueueLoadBitmapHook(const HMODULE managerModuleHandle, const HMODULE pluginModuleHandle) {
        if (hookQueued) {
            return true;
        }

        if (managerModuleHandle == nullptr || pluginModuleHandle == nullptr) {
            return false;
        }
        managerModule = managerModuleHandle;
        pluginModule = pluginModuleHandle;

        MH_STATUS mh_status =
            MH_CreateHookApiEx(L"user32.dll", "LoadBitmapA", reinterpret_cast<void *>(&HookedLoadBitmapA),
                               reinterpret_cast<void **>(&originalLoadBitmapA), &originalLoadBitmapEntry);
        if (mh_status != MH_OK) {
            LogMinHookError("LoadBitmapHook:MH_CreateHookApiEx", mh_status);
            originalLoadBitmapA = nullptr;
            originalLoadBitmapEntry = nullptr;
            managerModule = nullptr;
            pluginModule = nullptr;
            return false;
        }

        mh_status = MH_QueueEnableHook(originalLoadBitmapEntry);
        if (mh_status != MH_OK) {
            LogMinHookError("LoadBitmapHook:MH_QueueEnableHook", mh_status);
            MH_RemoveHook(originalLoadBitmapEntry);
            originalLoadBitmapA = nullptr;
            originalLoadBitmapEntry = nullptr;
            managerModule = nullptr;
            pluginModule = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
