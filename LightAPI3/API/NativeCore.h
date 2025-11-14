#pragma once

#include <windows.h>
#include <unordered_map>
#include <print>
#include <Psapi.h>

#ifdef LIGHT_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

using HookID = size_t;

class NATIVECORE_API NativeCore {
public:
	static uintptr_t findSig(const char* sig); // Find a signature in minecrafts executable

	static HookID hookFunction(uintptr_t address, void* hook, void** original); // Hook a function (PolyHook)
	static bool unhookFunction(HookID hook); // Unhook a function (PolyHook)

    static HMODULE getModule(void* addr); // Get a module from an address
};

#ifndef LIGHT_EXPORTS

// forwarded from polyhook
namespace PLH {
    template<typename FnCastTo>
    FnCastTo FnCast(uint64_t fnToCast, FnCastTo) {
        return (FnCastTo)fnToCast;
    }

    template<typename FnCastTo>
    FnCastTo FnCast(void* fnToCast, FnCastTo) {
        return (FnCastTo)fnToCast;
    }
}

#endif
