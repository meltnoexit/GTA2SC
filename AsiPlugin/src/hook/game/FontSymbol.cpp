#include "FontSymbol.h"

#include <string_view>

#include "hook/util/PatternScanner.h"

namespace GTA2SC {
    namespace {
        constexpr std::string_view kFontSelectionGlobalsPattern =
            "66 A1 ?? ?? ?? ?? 66 89 86 1C 01 00 00 5E C3 66 8B 0D ?? ?? ?? ?? 66 89 8E 1C 01 00 00 5E C3 90";
        constexpr std::string_view kBindingGlobalsPattern = "8B 0D ?? ?? ?? ?? 57 6A 07 E8 ?? ?? ?? ?? 89 46 14 66 8B "
                                                            "15 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 6A 00 52 E8 ?? ?? ?? ??";
        constexpr std::string_view kKanjiTextureBindingPattern =
            "66 A1 ?? ?? ?? ?? 56 57 8B F1 8B 0D ?? ?? ?? ?? 6A 00 50 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B F8";

        bool symbolsResolved = false;

        template <typename T> T ReadAbsoluteAddress32(const std::uint8_t *match, const std::size_t offset) {
            std::uint32_t address = 0;
            std::memcpy(&address, match + offset, sizeof(address));
            return reinterpret_cast<T>(address);
        }
    }

    const std::uint16_t *smallFontId = nullptr;
    const std::uint16_t *largeFontId = nullptr;

    void **styleSlot = nullptr;
    void **spriteManagerSlot = nullptr;
    void(__thiscall *kanjiTextureBinding)(Kanji *) = nullptr;

    bool ResolveFontSymbol(const std::span<const std::uint8_t> textSection) {
        if (symbolsResolved) {
            return true;
        }

        const auto *fontSelection = FindPattern(textSection, kFontSelectionGlobalsPattern);
        const auto *bindingGlobals = FindPattern(textSection, kBindingGlobalsPattern);
        const auto *textureBinding = FindPattern(textSection, kKanjiTextureBindingPattern);
        if (fontSelection == nullptr || bindingGlobals == nullptr || textureBinding == nullptr) {
            return false;
        }

        smallFontId = ReadAbsoluteAddress32<const std::uint16_t *>(fontSelection, 2);
        largeFontId = ReadAbsoluteAddress32<const std::uint16_t *>(bindingGlobals, 20);

        styleSlot = ReadAbsoluteAddress32<void **>(bindingGlobals, 26);
        spriteManagerSlot = ReadAbsoluteAddress32<void **>(bindingGlobals, 2);
        kanjiTextureBinding =
            reinterpret_cast<decltype(kanjiTextureBinding)>(const_cast<std::uint8_t *>(textureBinding));

        symbolsResolved = smallFontId && largeFontId && styleSlot && spriteManagerSlot && kanjiTextureBinding;
        return symbolsResolved;
    }
}
