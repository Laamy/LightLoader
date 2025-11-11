#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <print>
#include <fstream>
#include <format>
#include <sstream>
#include <Psapi.h>

#include "API/Console.h"

template <typename... Args>
void log(std::format_string<Args...> fmt, Args&&... args) {
    std::string msg = std::format(fmt, std::forward<Args>(args)...);

    std::cout << msg << std::flush;
    if (Console::logFile.is_open()) {
        Console::logFile << msg << std::flush;
    }
}

#include "API/NativeCore.h"
#include "API/GameConfig.h"
#include "API/FileIO.h"
#include "API/ModAPI.h"

#include "API/Events/GameEvents.h"

template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }

#define LIGHTAPI_VERSION L"LightLoader v1.1.0"

#endif //PCH_H
