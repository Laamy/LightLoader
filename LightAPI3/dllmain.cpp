#include "pch.h"
#include "proxy.h"

#include "LoadOverlay.h"
#include "Hooks/WndProcHook.h"

void ConsoleHandler(DWORD signal) {
	if (signal == CTRL_CLOSE_EVENT) {
		ClosingEvent dispatch;
		GameEvents::dispatch(&dispatch);
	}
}

FILE* f;
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	static std::once_flag initFlag;
	std::call_once(initFlag, [] {
		DllProxy::Initialize();
		AllocConsole();
		freopen_s(&f, "CONOUT$", "w", stdout);
		SetConsoleTitleA("LightAPI V3"); // bruh

		SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);//grr

		// basics just incase it crashes
		{
			log("-----------------------------------\n");
			log("Minecraft {}\n", GameConfig::getGameVersionA());
			//log("OS: {}\n", GetOS());
			log("-----------------------------------\n");
		}

		LoadResources();

		InitModLoader();

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitMods, 0, 0, 0);
	});

	return TRUE;
}
