#ifndef GTA2SC_ASIPLUGIN_PATTERNSCANNER_H
#define GTA2SC_ASIPLUGIN_PATTERNSCANNER_H

#include <Windows.h>

#include <cstdint>
#include <span>
#include <string_view>

namespace GTA2SC {
    std::span<const std::uint8_t> GetTextSection(HMODULE moduleHandle);
    const std::uint8_t *FindPattern(std::span<const std::uint8_t> text, std::string_view pattern);
    std::uintptr_t FindAbsoluteAddress32(std::span<const std::uint8_t> text, std::string_view pattern,
                                         std::size_t addressOffset);
}

#endif
