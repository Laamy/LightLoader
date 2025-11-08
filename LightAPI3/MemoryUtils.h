#pragma once

template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }

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
		std::print("Failed to create hook {}", address);
		//Console::Log("NativeCore", "Failed to create hook for address: 0x%p", address);
		return false;
	}

	if (MH_EnableHookEx(hookID, reinterpret_cast<LPVOID>(address)) != MH_OK) {
		std::print("Failed to enable hook {}", address);
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