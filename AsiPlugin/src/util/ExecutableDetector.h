#ifndef GTA2SC_ASIPLUGIN_EXECUTABLEDETECTOR_H
#define GTA2SC_ASIPLUGIN_EXECUTABLEDETECTOR_H

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string_view>

namespace GTA2SC {
    class ExecutableDetector {
        public:
            enum class Executable {
                Unknown,
                Game,
                Manager
            };

            enum class Version {
                Unknown,
                V96F,  // Version 9.6 Free
                V103N, // Version 10.3 NoCD
                V1144  // Version 11.44
            };

            struct ExecutableIdentity {
                    Executable executable;
                    Version version;
            };

            static std::optional<ExecutableIdentity> Detect();

        private:
            struct DetectionRule {
                    std::wstring_view filename;
                    uint64_t hash;
                    ExecutableIdentity identity;
            };

            static constexpr ExecutableIdentity kUnknownIdentity{.executable = Executable::Unknown,
                                                                 .version = Version::Unknown};
            static constexpr std::array kDetectionRules{
                DetectionRule{.filename = L"gta2.exe",
                              .hash = 0x541a9247d5997521,
                              .identity = {.executable = Executable::Game, .version = Version::V96F}},
                DetectionRule{.filename = L"gta2.exe",
                              .hash = 0xe0df7ba53d7c9ea7,
                              .identity = {.executable = Executable::Game, .version = Version::V103N}},
                DetectionRule{.filename = L"gta2.exe",
                              .hash = 0x4852ba313404f281,
                              .identity = {.executable = Executable::Game, .version = Version::V1144}},
                DetectionRule{.filename = L"gta2 manager.exe",
                              .hash = 0x18068064f4a5b553,
                              .identity = {.executable = Executable::Manager, .version = Version::V96F}},
                DetectionRule{.filename = L"gta2 manager.exe",
                              .hash = 0xae7e79da506d5000,
                              .identity = {.executable = Executable::Manager, .version = Version::V103N}},
                DetectionRule{.filename = L"gta2manager.exe",
                              .hash = 0xc90c4048a7ad8696,
                              .identity = {.executable = Executable::Manager, .version = Version::V1144}}};

            static std::optional<std::filesystem::path> GetExecutablePath();

            static bool MatchFilename(std::wstring_view actualFilename, std::wstring_view expectedFilename);

            static std::optional<uint64_t> CalculateXXH364(const std::filesystem::path &path);
    };
}

#endif
