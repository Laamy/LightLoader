#include "pch.h"
#include "proxy.h"

#include "LoadOverlay.h"
#include "Hooks/WndProcHook.h"

FILE* f;
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	static std::once_flag initFlag;
	std::call_once(initFlag, [] {
		DllProxy::Initialize();
		AllocConsole();
		freopen_s(&f, "CONOUT$", "w", stdout);
		SetConsoleTitleA("LightAPI V3"); // bruh

		// basics just incase it crashes
		{
			log("-----------------------------------\n");
			log("Minecraft {}\n", GameConfig::getMinecraftFolderA());
			//log("OS: {}\n", GetOS());
			log("-----------------------------------\n");
		}

		LoadResources();

		InitModLoader();

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitMods, 0, 0, 0);
	});

	return TRUE;
}
