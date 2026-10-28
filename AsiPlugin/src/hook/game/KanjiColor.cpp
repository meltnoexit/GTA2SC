#include "KanjiColor.h"

namespace GTA2SC {
    namespace {
        constexpr KanjiPaletteColor kBlack{.foreground = 0xB8};
        constexpr KanjiPaletteColor kBlue{.foreground = 0xFC};
        constexpr KanjiPaletteColor kRed{.foreground = 0x27};
        constexpr KanjiPaletteColor kWhite{.foreground = 0xFF};
        constexpr KanjiPaletteColor kYellow{.foreground = 0x47};

        constexpr std::array<KanjiPaletteColor, 16> kFrontendRemaps{{{.foreground = 0x88, .edge = 0x83},
                                                                     kWhite,
                                                                     kWhite,
                                                                     {.foreground = 0x89, .edge = 0x8D},
                                                                     kRed,
                                                                     kWhite,
                                                                     {.foreground = 0x84, .edge = 0x8D},
                                                                     {.foreground = 0x69, .edge = 0x6D},
                                                                     {.foreground = 0x1A},
                                                                     kWhite,
                                                                     kRed,
                                                                     kRed,
                                                                     {.foreground = 0x47, .edge = 0x4C},
                                                                     {.foreground = 0x84, .edge = 0x8D},
                                                                     {.foreground = 0x69, .edge = 0x6D},
                                                                     {.foreground = 0x47, .edge = 0x4C}}};

        constexpr auto kGameRemaps = [] {
            std::array<KanjiPaletteColor, 16> colors;
            colors.fill(kYellow);
            colors[0] = {.foreground = 0x88};
            colors[5] = kRed;
            return colors;
        }();

        constexpr auto kMissionTitleRemaps = [] {
            std::array<KanjiPaletteColor, 16> colors;
            colors.fill(kYellow);
            return colors;
        }();

        constexpr auto kPlayerNameRemaps = [] {
            auto colors = kGameRemaps;
            colors[0] = {.foreground = 0x7C};
            colors[1] = {.foreground = 0x0E};
            colors[2] = {.foreground = 0x69};
            colors[3] = {.foreground = 0x96};
            colors[4] = {.foreground = 0x37};
            colors[5] = kRed;
            colors[6] = {.foreground = 0x19};
            colors[7] = {.foreground = 0x4A};
            return colors;
        }();

        constexpr auto kSelectedFrontendRemaps = [] {
            auto colors = kFrontendRemaps;
            colors[8] = kWhite;
            return colors;
        }();

        constexpr auto kVehicleRemaps = [] {
            std::array<KanjiPaletteColor, 16> colors;
            colors.fill(kBlue);
            return colors;
        }();

        thread_local std::uint8_t currentDrawSources = 0;
        thread_local const KanjiColorProfile *currentProfile = nullptr;

        bool HasDrawSource(KanjiDrawSource source) noexcept {
            return (currentDrawSources & static_cast<std::uint8_t>(source)) != 0;
        }

        KanjiColorProfile SelectProfile(std::uint16_t fontId, std::uint16_t smallKanjiFontId) noexcept {
            if (smallKanjiFontId == 1) {
                KanjiColorProfile profile{kWhite, &kFrontendRemaps, kWhite};
                if (fontId == 5) {
                    profile.normal = kRed;
                    profile.remaps = &kSelectedFrontendRemaps;
                } else if (fontId == 11) {
                    profile.normal = {0x84};
                }
                return profile;
            }
            if (smallKanjiFontId != 3) {
                return {{}, nullptr, {}};
            }

            KanjiColorProfile profile{fontId == 4 || fontId == 6 ? kWhite : kYellow, &kGameRemaps, kYellow};
            if (HasDrawSource(KanjiDrawSource::MissionTitle)) {
                profile = {.normal = kYellow, .remaps = &kMissionTitleRemaps, .fallback = kYellow};
            } else if (HasDrawSource(KanjiDrawSource::VehicleBanner)) {
                profile = {.normal = kBlue, .remaps = &kVehicleRemaps, .fallback = kBlue};
            } else if (HasDrawSource(KanjiDrawSource::PlayerName)) {
                profile = {.normal = kYellow, .remaps = &kPlayerNameRemaps, .fallback = kYellow};
            }
            if (HasDrawSource(KanjiDrawSource::ZoneBanner)) {
                profile.normal = kBlack;
            }
            return profile;
        }

        void ApplyColor(Kanji *kanji, KanjiPaletteColor color) noexcept {
            kanji->foreground = color.foreground;
            kanji->edge = color.edge;
        }
    }

    KanjiDrawSourceScope::KanjiDrawSourceScope(KanjiDrawSource source) noexcept : previous_(currentDrawSources) {
        currentDrawSources |= static_cast<std::uint8_t>(source);
    }

    KanjiDrawSourceScope::~KanjiDrawSourceScope() {
        currentDrawSources = previous_;
    }

    KanjiColorScope::KanjiColorScope(std::uint16_t fontId, std::uint16_t smallKanjiFontId) noexcept
        : profile_(SelectProfile(fontId, smallKanjiFontId)), previous_(currentProfile) {
        currentProfile = profile_.remaps != nullptr ? &profile_ : nullptr;
    }

    KanjiColorScope::~KanjiColorScope() {
        currentProfile = previous_;
    }

    bool ApplyKanjiDefaultColor(Kanji *kanji) noexcept {
        if (kanji == nullptr || currentProfile == nullptr) {
            return false;
        }
        ApplyColor(kanji, currentProfile->normal);
        return true;
    }

    bool ApplyKanjiExplicitColor(Kanji *kanji, std::uint32_t colorId) noexcept {
        if (kanji == nullptr || currentProfile == nullptr) {
            return false;
        }
        const auto remap = static_cast<std::uint16_t>(colorId);
        const auto &colors = *currentProfile->remaps;
        ApplyColor(kanji, remap < colors.size() ? colors[remap] : currentProfile->fallback);
        return true;
    }
}
