#include "pch.h"
#include "proxy.h"

#include "LoadOverlay.h"

FILE* f;
void Entry() {
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA("LightAPI V3"); // bruh

    InitOverlay();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    static bool init = false;
    if (!init) {
        init = true;
        DllProxy::Initialize();
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Entry, 0, 0, 0); // might force it to wait idk yet
    }
    return TRUE;
}
