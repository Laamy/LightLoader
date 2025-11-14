#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <format>
#include <vector>
#include <unordered_map>

#ifdef LIGHT_EXPORTS
#define MODAPI_API __declspec(dllexport)
#else
#define MODAPI_API __declspec(dllimport)
#endif

class MODAPI_API ModConfig {
public:
	std::unordered_map<std::string, std::string> Settings{}; // whatever the mod(s) want to do with this
};

class MODAPI_API ModInfo {
public:
	bool IsLoaded = false; // True if the mod has been successfully loaded (No dependencies missing, etc)

	HMODULE Handle = nullptr; // Mod's module handle
	std::string Path{}; // Path to the mod file relative to the minecraft executable

	bool IsSupported = false; // True if the mod interfaces with ModAPI
	std::shared_ptr<ModConfig> Config; // (Requires IsSupported) Mod's configuration & other bits of data
};

class MODAPI_API ModAPI {
private:
	static std::string _err;

public:
	static std::vector<std::shared_ptr<ModInfo>> Mods; // List of mods from all sources (ModAPI, loaded, external, etc)

public:
	static bool loadDependencyW(std::wstring dll);
	static bool loadDependencyA(std::string dll);

	static std::vector<std::wstring> loadDependenciesW(std::vector<std::wstring> dlls);
	static std::vector<std::string> loadDependenciesA(std::vector<std::string> dlls);

	static std::string hasError();
	static void error(std::string err);

	static void initMod(HMODULE mod); // Initialize a mod within ModAPI
	static std::shared_ptr<ModInfo> getMod(HMODULE mod); // Retrieve a mod's ModInfo from its module handle
};
