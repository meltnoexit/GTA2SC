#include "PatternScanner.h"

#include <algorithm>
#include <charconv>
#include <string_view>
#include <vector>

namespace GTA2SC {
    namespace {
        std::vector<int> ParsePattern(std::string_view pattern) {
            std::vector<int> bytes;

            while (!pattern.empty()) {
                const auto start = pattern.find_first_not_of(" \t");
                if (start == std::string_view::npos) {
                    break;
                }
                pattern.remove_prefix(start);

                const auto end = pattern.find_first_of(" \t");
                const auto token = pattern.substr(0, end);
                pattern.remove_prefix(token.size());

                if (token == "??") {
                    bytes.push_back(-1);
                    continue;
                }

                if (token.size() != 2) {
                    return {};
                }

                unsigned value = 0;
                const auto [ptr, error] = std::from_chars(token.data(), token.data() + token.size(), value, 16);
                if (error != std::errc{} || ptr != token.data() + token.size()) {
                    return {};
                }

                bytes.push_back(static_cast<int>(value));
            }

            return bytes;
        }
    }

    const std::uint8_t *FindPattern(const std::span<const std::uint8_t> text, const std::string_view pattern) {
        const auto bytes = ParsePattern(pattern);
        if (bytes.empty() || text.size() < bytes.size()) {
            return nullptr;
        }

        const std::uint8_t *found = nullptr;
        for (std::size_t i = 0; i <= text.size() - bytes.size(); ++i) {
            bool matches = true;
            for (std::size_t j = 0; j < bytes.size(); ++j) {
                if (bytes[j] >= 0 && text[i + j] != bytes[j]) {
                    matches = false;
                    break;
                }
            }

            if (matches) {
                if (found != nullptr) {
                    return nullptr;
                }
                found = text.data() + i;
            }
        }
        return found;
    }

    std::span<const std::uint8_t> GetTextSection(const HMODULE moduleHandle) {
        if (moduleHandle == nullptr) {
            return {};
        }

        const auto *base = reinterpret_cast<const std::uint8_t *>(moduleHandle);
        const auto *dos = reinterpret_cast<const IMAGE_DOS_HEADER *>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) {
            return {};
        }

        const auto *nt = reinterpret_cast<const IMAGE_NT_HEADERS32 *>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            return {};
        }

        const auto *sections = IMAGE_FIRST_SECTION(nt);
        for (WORD index = 0; index < nt->FileHeader.NumberOfSections; ++index) {
            const auto &section = sections[index];
            if (std::memcmp(section.Name, ".text", sizeof(".text")) != 0) {
                continue;
            }

            const std::size_t offset = section.VirtualAddress;
            const std::size_t imageSize = nt->OptionalHeader.SizeOfImage;
            if (offset >= imageSize) {
                return {};
            }

            const std::size_t size = std::min<std::size_t>(section.Misc.VirtualSize, imageSize - offset);
            return {base + offset, size};
        }

        return {};
    }

    std::uintptr_t FindAbsoluteAddress32(const std::span<const std::uint8_t> text, const std::string_view pattern,
                                         const std::size_t addressOffset) {
        const auto *match = FindPattern(text, pattern);
        if (match == nullptr) {
            return 0;
        }

        const auto remaining = text.size() - static_cast<std::size_t>(match - text.data());
        if (addressOffset > remaining || remaining - addressOffset < sizeof(std::uint32_t)) {
            return 0;
        }

        std::uint32_t address = 0;
        std::memcpy(&address, match + addressOffset, sizeof(address));
        return address;
    }
}
