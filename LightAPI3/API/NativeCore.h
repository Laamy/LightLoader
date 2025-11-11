#pragma once

#include <windows.h>
#include <unordered_map>
#include <print>
#include <Psapi.h>
#include "../Libs/minhook/minhook.h"

#ifdef LIGHT_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

class NATIVECORE_API NativeCore {
private:
    static inline std::unordered_map<uintptr_t, int> hookIds{};

public: // some misc things
    static uintptr_t findSig(const char* sig);

    // HookFunction so minhook isnt essential to install into mods (also i want to multiplex them later)
    static bool hookFunction(uintptr_t address, void* hook, void** original);
    static bool unhookFunction(uintptr_t address);
};
