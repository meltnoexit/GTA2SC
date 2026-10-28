#include "ExecutableDetector.h"

#include <Windows.h>

#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <xxhash.h>

namespace GTA2SC {
    std::optional<std::filesystem::path> ExecutableDetector::GetExecutablePath() {
        std::wstring buffer(32768, L'\0');
        const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), buffer.size());
        if (length == 0 || length >= buffer.size()) {
            return std::nullopt;
        }
        buffer.resize(length);
        return std::filesystem::path(buffer);
    }

    bool ExecutableDetector::MatchFilename(const std::wstring_view actualFilename,
                                           const std::wstring_view expectedFilename) {
        return CompareStringOrdinal(actualFilename.data(), static_cast<int>(actualFilename.size()),
                                    expectedFilename.data(), static_cast<int>(expectedFilename.size()),
                                    TRUE) == CSTR_EQUAL;
    }

    std::optional<uint64_t> ExecutableDetector::CalculateXXH364(const std::filesystem::path &path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return std::nullopt;
        }
        const std::vector data(std::istreambuf_iterator{file}, std::istreambuf_iterator<char>{});
        if (file.bad()) {
            return std::nullopt;
        }
        return XXH3_64bits(data.data(), data.size());
    }

    std::optional<ExecutableDetector::ExecutableIdentity> ExecutableDetector::Detect() {
        const auto path = GetExecutablePath();
        if (!path) {
            return std::nullopt;
        }

        const auto filename = path->filename().wstring();
        bool knownFilename = false;
        for (const auto &rule : kDetectionRules) {
            if (MatchFilename(filename, rule.filename)) {
                knownFilename = true;
                break;
            }
        }
        if (!knownFilename) {
            return kUnknownIdentity;
        }

        const auto hash = CalculateXXH364(*path);
        if (!hash) {
            return std::nullopt;
        }

        for (const auto &rule : kDetectionRules) {
            if (MatchFilename(filename, rule.filename) && *hash == rule.hash) {
                return rule.identity;
            }
        }

        return kUnknownIdentity;
    }
}
