#ifndef GTA2SC_ASIPLUGIN_KANJI_H
#define GTA2SC_ASIPLUGIN_KANJI_H

#include <cstdint>

namespace GTA2SC {
    struct Kanji {
            void *index;
            void *bitmap;
            std::uint32_t indexCount;
            std::uint32_t unknown0C;
            void *smallResource;
            void *smallTexture;
            std::uint32_t unknown18;
            void *largeResource;
            void *largeTexture;
            std::uint8_t foreground;
            std::uint8_t edge;
            std::uint8_t unknown26[2];
    };
}

#endif
