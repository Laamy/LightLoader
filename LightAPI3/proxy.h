#pragma once

#include "Libs/QuickDllProxy/DllProxy.h"

void* ResolveModule()
{
    return LoadLibraryA("C:\\windows\\system32\\winhttp.dll");
}

bool ResolveFunction(void* Module, uint32_t Ordinal, const char* Name, void** Pointer)
{
    auto ptr = GetProcAddress((HMODULE)Module, Name);

    if (!ptr)
        return false;

    *Pointer = ptr;
    return true;
}

void HandleException(DllProxy::ErrorCode Code)
{
    char reason[32];
    sprintf_s(reason, "Error code: %u", Code);

    MessageBoxA(nullptr, reason, "PROXY ERROR", MB_ICONERROR);
    __debugbreak();
}

#define DLL_PROXY_EXPORT_LISTING_FILE "winhttp_exports.inc"
#define DLL_PROXY_CHECK_MISSING_EXPORTS
#define DLL_PROXY_LIBRARY_RESOLVER_CALLBACK ResolveModule
#define DLL_PROXY_EXPORT_RESOLVER_CALLBACK ResolveFunction
#define DLL_PROXY_EXCEPTION_CALLBACK HandleException
#define DLL_PROXY_DECLARE_IMPLEMENTATION
#include "Libs/QuickDllProxy/DllProxy.h"
