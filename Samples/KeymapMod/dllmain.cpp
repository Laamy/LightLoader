#include "pch.h"

#include <Events/GameEventsUt.h>
#include <FileIO.h>
#include <Console.h>

// feel free to ignore the log file and use console like normal
template <typename... Args>
void log(std::format_string<Args...> fmt, Args&&... args) {
    std::string msg = std::format(fmt, std::forward<Args>(args)...);

    std::cout << msg << std::flush;
    if (Console::logFile.is_open()) {
        Console::logFile << msg << std::flush;
    }
}

void Entry() {
    // initalize mods folder under this name
    FileIO io("KeymapMod");

    // ensure required libraries are loaded
    LoadLibraryW(L"LightUt.dll");

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
