#ifndef GTA2SC_ASIPLUGIN_FLAGBITMAPREPLACEMENTHOOK_H
#define GTA2SC_ASIPLUGIN_FLAGBITMAPREPLACEMENTHOOK_H

#include <windows.h>

namespace GTA2SC {
    bool QueueLoadBitmapHook(HMODULE managerModuleHandle, HMODULE pluginModuleHandle);
}

#endif