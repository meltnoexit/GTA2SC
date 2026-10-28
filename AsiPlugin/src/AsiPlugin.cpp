#include <Windows.h>

#include <MinHook.h>

#include "hook/game/FontSymbol.h"
#include "hook/game/FrontendDestructorHook.h"
#include "hook/game/FrontendFontSelectionHook.h"
#include "hook/game/FrontendKanjiState.h"
#include "hook/game/GameConstructorHook.h"
#include "hook/game/GbhDrawQuadHook.h"
#include "hook/game/GbhFreePaletteHook.h"
#include "hook/game/GbhFreeTextureHook.h"
#include "hook/game/GlyphWidthHook.h"
#include "hook/game/KanjiDefaultColorHook.h"
#include "hook/game/KanjiExplicitColorHook.h"
#include "hook/game/KanjiOutlineHook.h"
#include "hook/game/KanjiTextureBindingHook.h"
#include "hook/game/MilesAilStartupHook.h"
#include "hook/game/MissionTitleDrawHook.h"
#include "hook/game/PlayerNameDrawHook.h"
#include "hook/game/TextDrawHook.h"
#include "hook/game/TextMeasureHook.h"
#include "hook/game/VehicleBannerDrawHook.h"
#include "hook/game/ZoneBannerDrawHook.h"
#include "hook/manager/LoadBitmapHook.h"
#include "hook/manager/LoadResourceHook.h"
#include "hook/util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "util/ExecutableDetector.h"

namespace {
    HMODULE executableHandle;
    HMODULE pluginHandle;
}

using namespace GTA2SC;

extern "C" __declspec(dllexport) void InitializeASI() {
    const MH_STATUS mh_status = MH_Initialize();
    if (mh_status != MH_OK && mh_status != MH_ERROR_ALREADY_INITIALIZED) {
        OutputDebugStringA(MH_StatusToString(mh_status));
        return;
    }

    const auto executableIdentity = ExecutableDetector::Detect();
    if (!executableIdentity) {
        OutputDebugStringA("GTA2SC: could not identify the executable.\n");
        return;
    }

    switch (executableIdentity->executable) {
        case ExecutableDetector::Executable::Unknown:
            MessageBoxW(nullptr, L"不支持的可执行文件！", L"GTA2SC", MB_OK);
            break;
        case ExecutableDetector::Executable::Game:
            if (executableIdentity->version == ExecutableDetector::Version::V96F ||
                executableIdentity->version == ExecutableDetector::Version::V103N ||
                executableIdentity->version == ExecutableDetector::Version::V1144) {
                const auto textSection = GetTextSection(executableHandle);

                if (!ResolveFontSymbol(textSection) || !ResolveFrontendKanjiSymbol(textSection)) {
                    OutputDebugStringA("GTA2SC: failed to resolve game symbols.\n");
                    break;
                }
                if (!QueueMilesStartupHook()) {
                    OutputDebugStringA("GTA2SC: failed to queue Miles startup hook.\n");
                    break;
                }
                if (!QueueKanjiTextureBindingHook(textSection)) {
                    OutputDebugStringA("GTA2SC: failed to queue Kanji texture binding hook.\n");
                    break;
                }
                if (!QueueKanjiOutlineHook(textSection)) {
                    OutputDebugStringA("GTA2SC: failed to queue Kanji outline hook.\n");
                    break;
                }
                if (!QueueFrontendDestructorHook(textSection) || !QueueFrontendFontSelectionHook(textSection) ||
                    !QueueGameConstructorHook(textSection)) {
                    OutputDebugStringA("GTA2SC: failed to queue frontend Kanji hooks.\n");
                    break;
                }
                if (!QueueKanjiDefaultColorHook(textSection) || !QueueKanjiExplicitColorHook(textSection) ||
                    !QueueMissionTitleDrawHook(textSection) || !QueuePlayerNameDrawHook(textSection) ||
                    !QueueVehicleBannerDrawHook(textSection) || !QueueZoneBannerDrawHook(textSection)) {
                    OutputDebugStringA("GTA2SC: failed to queue Kanji color hooks.\n");
                    break;
                }
                if (!QueueGlyphWidthHook(executableHandle, textSection) ||
                    !QueueTextDrawHook(executableHandle, textSection) ||
                    !QueueTextMeasureHook(executableHandle, textSection)) {
                    OutputDebugStringA("GTA2SC: failed to queue text rendering hooks.\n");
                    break;
                }
                if (GetModuleHandleW(L"d3ddll.dll") == nullptr && LoadLibraryW(L"d3ddll.dll") == nullptr) {
                    OutputDebugStringA("GTA2SC: failed to load d3ddll.dll.\n");
                }
                if (!QueueGbhFreeTextureHook() || !QueueGbhDrawQuadHook() || !QueueGbhFreePaletteHook()) {
                    OutputDebugStringA("GTA2SC: failed to queue Gbh rendering hooks.\n");
                }

                const MH_STATUS status = MH_ApplyQueued();
                if (status != MH_OK) {
                    LogMinHookError("GTA2SC:MH_ApplyQueued", status);
                    break;
                }
            } else {
                MessageBoxW(nullptr, L"不支持的游戏版本！", L"GTA2SC", MB_OK);
            }
            break;
        case ExecutableDetector::Executable::Manager:
            if (executableIdentity->version == ExecutableDetector::Version::V96F ||
                executableIdentity->version == ExecutableDetector::Version::V103N ||
                executableIdentity->version == ExecutableDetector::Version::V1144) {
                const auto textSection = GetTextSection(executableHandle);

                if (!QueueLoadBitmapHook(executableHandle, pluginHandle)) {
                    OutputDebugStringA("GTA2SC: failed to queue LoadBitmapA hook.\n");
                }
                if (!QueueLoadResourceHook(executableHandle, pluginHandle, textSection)) {
                    OutputDebugStringA("GTA2SC: failed to queue LoadResourceA hook.\n");
                }

                const MH_STATUS status = MH_ApplyQueued();
                if (status != MH_OK) {
                    LogMinHookError("GTA2SC: MH_ApplyQueued", status);
                }
            } else {
                MessageBoxW(nullptr, L"不支持的版本！", L"GTA2SC", MB_OK);
            }
            break;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        executableHandle = GetModuleHandleW(nullptr);
        pluginHandle = hinstDLL;
    }

    return TRUE;
}
