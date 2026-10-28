#include "FrontendKanjiState.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

#include "FontSymbol.h"
#include "hook/util/PatternScanner.h"

namespace GTA2SC {
    Kanji **activeKanjiSlot = nullptr;

    namespace {
        constexpr std::string_view kActiveKanjiPattern =
            "66 3B 35 ?? ?? ?? ?? 74 09 66 3B 35 ?? ?? ?? ?? 75 1C 8B 0D ?? ?? ?? ?? 83 FB 08 75 08 55 E8 ?? ?? ?? ??";
        constexpr std::string_view kGameNewPattern =
            "6A 01 FF 74 24 08 E8 ?? ?? ?? ?? 59 59 C3 55 8B EC 51 53 8B 45 0C 83 C0";
        constexpr std::string_view kGameFreePattern =
            "E9 ?? ?? ?? ?? 6A 01 FF 74 24 08 E8 ?? ?? ?? ?? 59 59 C3 55 8B EC 51 53";
        constexpr std::string_view kKanjiConstructorPattern =
            "56 8B F1 33 C0 C6 46 24 FE C6 46 25 F7 89 46 04 89 06 89 46 08 E8 ?? ?? ?? ?? 8B C6 5E C3 90 90";
        constexpr std::string_view kKanjiDestructorPattern =
            "56 8B F1 E8 F8 61 00 00 F6 44 24 08 01 74 09 56 E8 ?? ?? ?? ?? 83 C4 04 8B C6 5E C2 04 00 90 90";

        void *(__cdecl *gameNew)(std::size_t) = nullptr;
        void(__cdecl *gameFree)(void *) = nullptr;
        Kanji *(__thiscall *KanjiConstructor)(Kanji *) = nullptr;
        Kanji *(__thiscall *KanjiDestructor)(Kanji *, std::uint8_t) = nullptr;
        Kanji *frontendKanji = nullptr;
    }

    bool ResolveFrontendKanjiSymbol(const std::span<const std::uint8_t> textSection) {
        if (activeKanjiSlot && gameNew && gameFree && KanjiConstructor && KanjiDestructor) {
            return true;
        }

        activeKanjiSlot = reinterpret_cast<Kanji **>(FindAbsoluteAddress32(textSection, kActiveKanjiPattern, 20));
        gameNew =
            reinterpret_cast<decltype(gameNew)>(const_cast<std::uint8_t *>(FindPattern(textSection, kGameNewPattern)));
        gameFree = reinterpret_cast<decltype(gameFree)>(
            const_cast<std::uint8_t *>(FindPattern(textSection, kGameFreePattern)));
        KanjiConstructor = reinterpret_cast<decltype(KanjiConstructor)>(
            const_cast<std::uint8_t *>(FindPattern(textSection, kKanjiConstructorPattern)));
        KanjiDestructor = reinterpret_cast<decltype(KanjiDestructor)>(
            const_cast<std::uint8_t *>(FindPattern(textSection, kKanjiDestructorPattern)));

        return activeKanjiSlot && gameNew && gameFree && KanjiConstructor && KanjiDestructor;
    }

    bool InitializeFrontendKanji() {
        if (frontendKanji != nullptr && *activeKanjiSlot != frontendKanji) {
            ReleaseFrontendKanji();
        }

        if (*activeKanjiSlot != nullptr) {
            return true;
        }

        if (*styleSlot == nullptr || *spriteManagerSlot == nullptr) {
            return false;
        }

        auto *memory = static_cast<Kanji *>(gameNew(sizeof(Kanji)));
        if (memory == nullptr) {
            return false;
        }

        Kanji *kanji = KanjiConstructor(memory);
        if (kanji == nullptr) {
            gameFree(memory);
            return false;
        }

        kanjiTextureBinding(kanji);

        frontendKanji = kanji;
        *activeKanjiSlot = kanji;

        return true;
    }

    void ReleaseFrontendKanji() {
        if (frontendKanji == nullptr) {
            return;
        }

        if (*activeKanjiSlot == frontendKanji) {
            *activeKanjiSlot = nullptr;
        }

        KanjiDestructor(std::exchange(frontendKanji, nullptr), 1);
    }
}
