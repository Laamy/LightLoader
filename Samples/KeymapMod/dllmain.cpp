#include "pch.h"

#include <print>

#include <Events/GameEventsUt.h>
#include <FileIO.h>
#include <Console.h>
#include <ModAPI.h>

// feel free to ignore the log file and use console like normal
template <typename... Args>
void log(std::format_string<Args...> fmt, Args&&... args) {
    std::string msg = std::format(fmt, std::forward<Args>(args)...);

    std::cout << msg << std::flush;
    if (Console::logFile.is_open()) {
        Console::logFile << msg << std::flush;
    }
}

// initalize mod folder under this name
FileIO io("KeymapMod");
void Entry() {
    // ensure required libraries are loaded
    if (!ModAPI::LoadDependenciesW({ L"LightUt.dll" }))
        return ModAPI::Error("Unable to load dependencies");

    GameEvents::subscribe(EventID::WndProc, [](BaseEvent* _event) {
        auto event = reinterpret_cast<WndProcEvent*>(_event);

        //log("Msg {}", event->Msg);

        switch (event->Msg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            log("Keydown {}\n", event->wParam);
            break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            log("Keyup {}\n", event->wParam);
            break;

        case WM_MOUSEMOVE:
            int x = LOWORD(event->lParam);
            int y = HIWORD(event->lParam);
            log("Mouse moved {}, {}\n", x, y);
            break;
        }
    });

    GameEvents::subscribe((EventID)EventIDUt::RakNetSendToServer, [](BaseEvent* _event) {
        auto event = reinterpret_cast<RakNetSendToServer*>(_event);

        log("Packet {} sent\n", event->Packet->getName());
    });

    GameEvents::subscribe(EventID::Loaded, [](BaseEvent*) {
        auto ini = io.getIniStream("test.ini");

        auto v1 = ini.GetOrDefault("TextThing", std::string("Hello, World!"));
        std::print("TextThing {}\n", v1);

        auto v2 = ini.GetOrDefault("FloatThing", 1.f);
        std::print("FloatThing {}\n", v2);

        log("Mod Loader final\n");
    });

    GameEvents::subscribe(EventID::Closing, [](BaseEvent*) {
        log("Game quit called\n");
    });

    log("KeymapMod sample loaded\n");
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
