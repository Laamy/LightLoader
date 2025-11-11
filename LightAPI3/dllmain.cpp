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

static FILE* f = nullptr;
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	static std::once_flag initFlag;
	std::call_once(initFlag, [] {
		DllProxy::Initialize();
		AllocConsole();

		freopen_s(&f, "CONOUT$", "w", stdout);
		SetConsoleTitleA("LightAPI V3"); // bruh

		SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);//grr

		log("-----------------------------------\n");
		log("Minecraft {}\n", GameConfig::getGameVersionA());
		//log("OS: {}\n", GetOS());
		log("-----------------------------------\n");

		LoadResources();
		InitModLoader();

		CreateThread(nullptr, 0,
					 reinterpret_cast<LPTHREAD_START_ROUTINE>(InitMods),
					 nullptr, 0, nullptr);
	});

	return TRUE;
}
