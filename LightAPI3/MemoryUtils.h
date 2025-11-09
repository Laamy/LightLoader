#pragma once

template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }

static uintptr_t range_start = (uintptr_t)GetModuleHandleW(L"Minecraft.Windows.exe");

#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BYTE(x) ((GET_BITS(x[0]) << 4) | GET_BITS(x[1]))
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? (x - '0') : 0))

inline uintptr_t findSig(const char* sig) {

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

    for (uintptr_t p_cur = range_start; p_cur < range_end; p_cur++) // eevrything we use is over 0x100000 so we can just skip that part of memory
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

static std::unordered_map<uintptr_t, int> hookIds{};
// HookFunction so minhook isnt essential to install into mods (also i want to multiplex them later)
inline bool HookFunction(uintptr_t address, void* hook, void** original) {
	static bool initialized = false;
	if (!initialized) {
		MH_Initialize(); // wont confirm this because kiero and other crap can initialize it before us (doubtful though)
		initialized = true;
	}

	if (hookIds.find(address) == hookIds.end())
		hookIds[address] = 0;

	auto hookID = hookIds[address]++;

	if (MH_CreateHookEx(hookID, reinterpret_cast<LPVOID>(address), hook, original) != MH_OK) {
		log("Failed to create hook {}", address);
		//Console::Log("NativeCore", "Failed to create hook for address: 0x%p", address);
		return false;
	}

	if (MH_EnableHookEx(hookID, reinterpret_cast<LPVOID>(address)) != MH_OK) {
		log("Failed to enable hook {}", address);
		//Console::Log("NativeCore", "Failed to enable hook for address: 0x%p", address);
		return false;
	}

	return true;
}

inline bool UnhookFunction(uintptr_t address) {
	if (MH_DisableHook(reinterpret_cast<LPVOID>(address)) != MH_OK) {
		//Console::Log("NativeCore", "Failed to disable hook for address: 0x%p", address);
		return false;
	}

	if (MH_RemoveHook(reinterpret_cast<LPVOID>(address)) != MH_OK) {
		//Console::Log("NativeCore", "Failed to remove hook for address: 0x%p", address);
		return false;
	}

	return true;
}