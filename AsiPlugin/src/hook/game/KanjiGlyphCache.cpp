#include "KanjiGlyphCache.h"

#include <xxhash.h>

#include <array>

namespace GTA2SC {
    namespace {
        constexpr std::size_t kBitmapStride = 256;
        constexpr std::size_t kMaximumWidth = 32;
        constexpr std::size_t kMaximumHeight = 34;
        constexpr std::size_t kSetCount = 128;
        constexpr std::size_t kWays = 8;

        struct GlyphEntry {
            GbhTexture *owner = nullptr;
            GbhTexture *texture = nullptr;
            std::uint64_t hash = 0;
            std::uint64_t used = 0;
            std::uint32_t drawFlags = 0;
            std::array<std::uint8_t, kBitmapStride * kMaximumHeight> bitmap{};
        };

        std::array<std::array<GlyphEntry, kWays>, kSetCount> glyphs;
        std::uint64_t useCounter = 0;

        void ReleaseGlyph(GlyphEntry &entry, FreeGbhTextureFn freeTexture) {
            if (entry.texture != nullptr) {
                freeTexture(entry.texture);
                entry.texture = nullptr;
            }
            entry.owner = nullptr;
            entry.used = 0;
        }
    }

    void ReleaseKanjiGlyphCache(FreeGbhTextureFn freeTexture, const GbhTexture *owner) {
        for (auto &set : glyphs) {
            for (auto &entry : set) {
                if (owner == nullptr || entry.owner == owner) {
                    ReleaseGlyph(entry, freeTexture);
                }
            }
        }
    }

    GbhTexture *GetCachedKanjiTexture(GbhTexture *source, const std::uint32_t drawFlags,
                                    RegisterGbhTextureFn registerTexture, FreeGbhTextureFn freeTexture) {
        if (source->pixels == nullptr || source->width == 0 || source->width > kMaximumWidth ||
            source->height == 0 || source->height > kMaximumHeight ||
            source->x + source->width > kBitmapStride || source->y + source->height > kBitmapStride ||
            (source->flags & 1) != 0) {
            return source;
        }

        std::array<std::uint8_t, kMaximumWidth * kMaximumHeight> pixels{};
        const auto *sourcePixels = source->pixels + source->y * kBitmapStride + source->x;
        for (std::size_t row = 0; row < source->height; ++row) {
            std::memcpy(pixels.data() + row * source->width, sourcePixels + row * kBitmapStride, source->width);
        }
        const auto hash = XXH3_64bits(pixels.data(), source->width * source->height);
        const auto setIndex = (hash ^ reinterpret_cast<std::uintptr_t>(source) ^ source->palette ^ drawFlags) % kSetCount;
        auto &set = glyphs[setIndex];
        auto *victim = &set.front();
        for (auto &entry : set) {
            if (entry.texture != nullptr && entry.owner == source && entry.hash == hash && entry.drawFlags == drawFlags &&
                entry.texture->width == source->width && entry.texture->height == source->height &&
                entry.texture->palette == source->palette && entry.texture->paletteType == source->paletteType &&
                entry.texture->paletteScale == source->paletteScale &&
                (entry.texture->flags & 0x80) == (source->flags & 0x80)) {
                bool equal = true;
                for (std::size_t row = 0; row < source->height; ++row) {
                    if (std::memcmp(entry.bitmap.data() + row * kBitmapStride, pixels.data() + row * source->width, source->width) != 0) {
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    entry.used = ++useCounter;
                    return entry.texture;
                }
            }
            if (entry.used < victim->used) {
                victim = &entry;
            }
        }

        ReleaseGlyph(*victim, freeTexture);
        victim->bitmap.fill(0);
        for (std::size_t row = 0; row < source->height; ++row) {
            std::memcpy(victim->bitmap.data() + row * kBitmapStride, pixels.data() + row * source->width, source->width);
        }
        auto *texture = registerTexture(source->width, source->height, victim->bitmap.data(), 0, (source->flags & 0x80) != 0);
        if (texture == nullptr) {
            return source;
        }
        texture->paletteType = source->paletteType;
        texture->paletteScale = source->paletteScale;
        texture->palette = source->palette;
        texture->flags = source->flags & 0xFE;
        victim->owner = source;
        victim->texture = texture;
        victim->hash = hash;
        victim->used = ++useCounter;
        victim->drawFlags = drawFlags;
        return texture;
    }
}
