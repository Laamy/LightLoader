#include "pch.h"

#include "API/Events/GameEventsUt.h"

#include "Hooks/RakNetHook.h"

#include <ModAPI.h>

void Entry() {
    // required to interface with other ModAPI mod's
    GameEvents::subscribe(EventID::Loaded, [&](BaseEvent*) {
		auto modInfo = ModAPI::getMod(NativeCore::getModule(&Entry));
        ModAPI::initMod(modInfo->Handle);

		modInfo->Config->Settings["Version"] = "1"; // I'll increase this everytime the LightUt api changes
	});

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
