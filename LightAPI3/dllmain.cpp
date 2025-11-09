#include "pch.h"
#include "proxy.h"

#include "LoadOverlay.h"

FILE* f;
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    static bool init = false;
    if (!init) {
        init = true;
        DllProxy::Initialize();
        AllocConsole();
        freopen_s(&f, "CONOUT$", "w", stdout);
        SetConsoleTitleA("LightAPI V3"); // bruh

        // basics just incase it crashes
        {
            log("-----------------------------------\n");
            log("Minecraft {}\n", extractVersion());
            log("-----------------------------------\n");
        }

        LoadResources();

        InitModLoader();

        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitMods, 0, 0, 0);
    }
    return TRUE;
}
