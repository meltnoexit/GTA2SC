#include "KanjiTextureBindingHook.h"

#include <cstdint>
#include <MinHook.h>
#include <string_view>

#include "../util/MinHookErrorLogger.h"
#include "FontSymbol.h"
#include "hook/util/PatternScanner.h"
#include "Kanji.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kFontSpritePattern =
            "8B 44 24 04 8B 49 1C 25 FF FF 00 00 66 8B 44 41 02 66 03 44 24 08 C2 08";
        constexpr std::string_view kSpriteTexturePattern =
            "8B 44 24 0C 83 C0 FE 83 F8 06 0F 87 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? 8B 44 24 08 8B 54 24 04 50";

        void(__thiscall *originalKanjiTextureBinding)(Kanji *) = nullptr;
        std::uint16_t(__thiscall *fontSprite)(void *, std::uint32_t, std::uint32_t) = nullptr;
        void *(__thiscall *spriteTexture)(void *, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) = nullptr;
        void *originalKanjiTextureBindingEntry = nullptr;

        bool hookQueued = false;

        void __fastcall HookedKanjiTextureBinding(Kanji *kanji, void * /* edx */) {
            originalKanjiTextureBinding(kanji);

            const auto smallFont = *smallFontId;
            if (smallFont != 1) {
                return;
            }

            void *style = *styleSlot;
            void *spriteManager = *spriteManagerSlot;
            if (style == nullptr || spriteManager == nullptr) {
                return;
            }

            const auto *styleBytes = static_cast<const std::uint8_t *>(style);
            const auto *paletteCounts = *reinterpret_cast<const std::uint16_t *const *>(styleBytes + 0x10);
            if (paletteCounts == nullptr || paletteCounts[7] <= 9) {
                return;
            }

            const auto largeFont = *largeFontId;
            void *smallTexture = spriteTexture(spriteManager, 7, fontSprite(style, smallFont, 0), 8, 9);
            void *largeTexture = spriteTexture(spriteManager, 7, fontSprite(style, largeFont, 0), 8, 9);
            if (smallTexture == nullptr || largeTexture == nullptr) {
                return;
            }

            kanji->smallTexture = smallTexture;
            kanji->largeTexture = largeTexture;
        }
    }

    bool QueueKanjiTextureBindingHook(const std::span<const std::uint8_t> textSection) {
        if (hookQueued) {
            return true;
        }

        fontSprite = reinterpret_cast<decltype(fontSprite)>(
            const_cast<std::uint8_t *>(FindPattern(textSection, kFontSpritePattern)));
        spriteTexture = reinterpret_cast<decltype(spriteTexture)>(
            const_cast<std::uint8_t *>(FindPattern(textSection, kSpriteTexturePattern)));
        if (fontSprite == nullptr || spriteTexture == nullptr) {
            fontSprite = nullptr;
            spriteTexture = nullptr;
            return false;
        }

        originalKanjiTextureBindingEntry = reinterpret_cast<void *>(kanjiTextureBinding);

        MH_STATUS status =
            MH_CreateHook(originalKanjiTextureBindingEntry, reinterpret_cast<void *>(&HookedKanjiTextureBinding),
                          reinterpret_cast<void **>(&originalKanjiTextureBinding));
        if (status != MH_OK) {
            LogMinHookError("KanjiTextureBindingHook: MH_CreateHook", status);
            originalKanjiTextureBinding = nullptr;
            originalKanjiTextureBindingEntry = nullptr;
            fontSprite = nullptr;
            spriteTexture = nullptr;
            return false;
        }

        status = MH_QueueEnableHook(originalKanjiTextureBindingEntry);
        if (status != MH_OK) {
            LogMinHookError("KanjiTextureBindingHook: MH_QueueEnableHook", status);
            MH_RemoveHook(originalKanjiTextureBindingEntry);
            originalKanjiTextureBinding = nullptr;
            originalKanjiTextureBindingEntry = nullptr;
            fontSprite = nullptr;
            spriteTexture = nullptr;
            return false;
        }

        hookQueued = true;
        return true;
    }
}
