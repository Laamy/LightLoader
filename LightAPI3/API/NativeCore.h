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

using HookID = size_t;

class NATIVECORE_API NativeCore {
public: // some misc things
    static uintptr_t findSig(const char* sig);

    // Returns a HookID you can use to call unhookFunction
    static HookID hookFunction(uintptr_t address, void* hook, void** original);
    static bool unhookFunction(HookID hook);

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
