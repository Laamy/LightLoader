#include "pch.h"

#include "API/Events/GameEventsUt.h"

#include "Hooks/RakNetHook.h"

void Entry() {
    InitRakNetHook();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        Entry();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
