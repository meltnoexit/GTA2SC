#include "MinHookErrorLogger.h"

#include <windows.h>

#include <cstdio>

namespace GTA2SC {
    void LogMinHookError(const char *operation, const MH_STATUS status) {
        char message[256];
        std::snprintf(message, sizeof(message), "GTA2SC: %s failed: %s\n", operation, MH_StatusToString(status));
        OutputDebugStringA(message);
    }
}
