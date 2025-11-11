#include "NativeCore.h"

static uintptr_t range_start = (uintptr_t)GetModuleHandleW(L"Minecraft.Windows.exe");

#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BYTE(x) ((GET_BITS(x[0]) << 4) | GET_BITS(x[1]))
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? (x - '0') : 0))

uintptr_t NativeCore::findSig(const char* sig)
{
    const char* pattern = sig;
    uintptr_t first_match = 0;
    static MODULEINFO mi_mod_info;
    static bool init = false;
    if (!init)
    {
        init = true;
        GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(range_start), &mi_mod_info, sizeof(MODULEINFO));
    }
    static const uintptr_t range_end = range_start + mi_mod_info.SizeOfImage;

    BYTE pat_byte = GET_BYTE(pattern);
    const char* old_pat = pattern;

    for (uintptr_t p_cur = range_start; p_cur < range_end; p_cur++)
    {
        if (!*pattern)
        {
            return first_match;
        }

        while (*(PBYTE)pattern == ' ')
        {
            pattern++;
        }

        if (!*pattern)
        {
            return first_match;
        }

        if (old_pat != pattern)
        {
            old_pat = pattern;
            if (*(PBYTE)pattern != '\?') {
                pat_byte = GET_BYTE(pattern);
            }
        }

        if (*(PBYTE)pattern == '\?' || *(BYTE*)p_cur == pat_byte)
        {
            if (!first_match)
            {
                first_match = p_cur;
            }

            if (!pattern[2])
            {
                return first_match;
            }

            pattern += 2;
        }
        else
        {
            pattern = sig;
            first_match = 0;
        }
    }

    return 0;
}

// my intelisense died here
#include <polyhook2/IHook.hpp>
#include <polyhook2/Detour/NatDetour.hpp>

std::unordered_map<size_t, std::unique_ptr<PLH::NatDetour>> hooks_;
size_t nextId_ = 1;
std::mutex mtx_;

HookID NativeCore::hookFunction(uintptr_t address, void* hook, void** original)
{
    auto detour = std::make_unique<PLH::NatDetour>(
        address,
        (uintptr_t)hook,
        (uintptr_t*)original);

    if (!detour->hook()) {
        std::print("Failed to create hook {:#X}\n", address);
        return 0;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    size_t id = nextId_++;
    hooks_.emplace(id, std::move(detour));
    return id;
}

bool NativeCore::unhookFunction(HookID hook)
{
    std::lock_guard<std::mutex> lock(mtx_);

    auto it = hooks_.find(hook);
    if (it == hooks_.end()) {
        std::print("No hook found for ID {}\n", hook);
        return false;
    }

    if (!it->second->unHook()) {
        std::print("Failed to disable hook ID {}\n", hook);
        return false;
    }

    hooks_.erase(it);
    return true;
}
