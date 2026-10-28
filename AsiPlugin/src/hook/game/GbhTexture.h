#ifndef GTA2SC_ASIPLUGIN_GBHTEXTURE_H
#define GTA2SC_ASIPLUGIN_GBHTEXTURE_H

#include <cstdint>

namespace GTA2SC {
    struct GbhTexture {
        std::uint16_t id;
        std::uint16_t unknown02;
        std::uint16_t paletteType;
        std::uint16_t lockedStride;
        void *lockedPixels;
        std::uint8_t x;
        std::uint8_t y;
        std::uint16_t width;
        std::uint16_t height;
        std::uint8_t paletteScale;
        std::uint8_t flags;
        const std::uint8_t *pixels;
        std::uint32_t palette;
        void *rendererCache;
    };
}

#endif
