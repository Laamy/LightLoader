#include "ModAPI.h"
#include "../Utils.h"

inline bool ModAPI::loadDependencyW(std::wstring dll) {
    auto res = LoadLibraryW(std::format(L"mods\\{}", dll).c_str());

    if (res != NULL) return true;

    std::wcout << std::format(L"Failed to load library {}, GetLastError; 0x{:#x}\n", dll, GetLastError());
    return false;
}

inline bool ModAPI::loadDependencyA(std::string dll) {
	return loadDependencyW(utf8_to_wstring(dll));
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
std::vector<std::wstring> ModAPI::loadDependenciesW(std::vector<std::wstring> dlls)
{
    std::vector<std::wstring> result;

    for (auto dll : dlls)
        if (!loadDependencyW(dll))
            result.push_back(dll);

    return result;
}

std::vector<std::string> ModAPI::loadDependenciesA(std::vector<std::string> dlls)
{
    std::vector<std::string> result;

    for (auto dll : dlls)
        if (!loadDependencyA(dll))
            result.push_back(dll);

    return result;
}

std::string ModAPI::hasError()
{
    return _err;
}

void ModAPI::error(std::string err)
{
    _err = err;
}

#include <print>
void ModAPI::initMod(HMODULE mod)
{
	std::shared_ptr<ModInfo> info = getMod(mod);

    if (!info)
		throw std::runtime_error("Mod not found in ModAPI::Mods");

    info->IsSupported = true;
	info->Config = std::make_shared<ModConfig>();
}

std::shared_ptr<ModInfo> ModAPI::getMod(HMODULE mod)
{
    for (auto m : Mods) {
        if (m->Handle == mod)
			return m;
    }

	return nullptr;
}

std::string ModAPI::_err = "";
std::vector<std::shared_ptr<ModInfo>> ModAPI::Mods = {};
