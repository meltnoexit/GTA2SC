#include "KanjiOutlineHook.h"

#include <cstdint>
#include <MinHook.h>
#include <string_view>

#include "../util/MinHookErrorLogger.h"
#include "hook/util/PatternScanner.h"
#include "Kanji.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kKanjiOutlinePattern =
            "53 8B 5C 24 10 56 57 33 FF 81 E3 FF FF 00 00 8B F1 0F 8E ?? ?? ?? ?? 8B 54 24 10 55 8B 6C 24 18";
        constexpr std::size_t kBitmapStride = 256;

        void(__thiscall *originalKanjiOutline)(Kanji *, std::uint8_t *, std::uint32_t, std::uint32_t) = nullptr;
        void *originalKanjiOutlineEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedKanjiOutline(Kanji *kanji, void * /* edx */, std::uint8_t *bitmap, std::uint32_t width,
                                           std::uint32_t height) {
            if (kanji == nullptr || bitmap == nullptr || kanji->edge != 0) {
                originalKanjiOutline(kanji, bitmap, width, height);
                return;
            }

            if (width == 16 && height == 17) {
                std::memset(bitmap + 16 * kBitmapStride, 0, width);
            } else if (width == 32 && height == 34) {
                std::memset(bitmap + 32 * kBitmapStride, 0, width);
                std::memset(bitmap + 33 * kBitmapStride, 0, width);
            } else {
                originalKanjiOutline(kanji, bitmap, width, height);
            }
        }
    }

    bool QueueKanjiOutlineHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        originalKanjiOutlineEntry = const_cast<std::uint8_t *>(FindPattern(textSection, kKanjiOutlinePattern));
        if (originalKanjiOutlineEntry == nullptr) {
            return false;
        }

        MH_STATUS status = MH_CreateHook(originalKanjiOutlineEntry, reinterpret_cast<void *>(&HookedKanjiOutline),
                                         reinterpret_cast<void **>(&originalKanjiOutline));
        if (status != MH_OK) {
            LogMinHookError("KanjiOutlineHook: MH_CreateHook", status);
            originalKanjiOutline = nullptr;
            originalKanjiOutlineEntry = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalKanjiOutlineEntry);
        if (status != MH_OK) {
            LogMinHookError("KanjiOutlineHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalKanjiOutlineEntry);
            originalKanjiOutline = nullptr;
            originalKanjiOutlineEntry = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
