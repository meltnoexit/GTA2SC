#include "KanjiFontSelection.h"

#include "FontSymbol.h"
#include "FrontendKanjiState.h"

namespace GTA2SC {
    namespace {
        constexpr std::size_t kMaxTextLength = 4096;

        std::uint32_t WithFontId(std::uint32_t argument, std::uint16_t fontId) {
            return (argument & 0xFFFF0000u) | fontId;
        }

        std::uint16_t GetKanjiGlyphWidth(const std::uint16_t fontId) {
            if (fontId == *smallFontId) {
                return 16;
            }
            if (fontId == *largeFontId) {
                return 32;
            }
            return 0;
        }
    }

    TextFontSelection SelectTextFont(const std::uint16_t *text, const std::uint32_t fontArgument) {
        TextFontSelection result{.fontArgument = fontArgument};
        if (text == nullptr || *activeKanjiSlot == nullptr) {
            return result;
        }

        const auto width = GetKanjiGlyphWidth(static_cast<std::uint16_t>(fontArgument));
        if (width != 0) {
            result.kanjiGlyphWidth = width;
            return result;
        }

        std::size_t length = 0;
        bool hasWideCharacter = false;

        while (length < kMaxTextLength && text[length] != 0) {
            hasWideCharacter |= text[length] > 0xFF;
            ++length;
        }

        if (length == kMaxTextLength) {
            return result;
        }

        if (hasWideCharacter) {
            result.fontArgument = WithFontId(fontArgument, *smallFontId);
            result.kanjiGlyphWidth = 16;
            return result;
        }

        return result;
    }

    GlyphFontSelection SelectGlyphFont(const std::uint16_t fontId, const std::uint16_t character) {
        GlyphFontSelection result{.fontId = fontId};
        if (*activeKanjiSlot == nullptr) {
            return result;
        }

        const auto width = GetKanjiGlyphWidth(fontId);
        if (width != 0) {
            result.kanjiGlyphWidth = width;
        } else if (character > 0xFF) {
            result.fontId = *smallFontId;
            result.kanjiGlyphWidth = 16;
        }

        return result;
    }
}
