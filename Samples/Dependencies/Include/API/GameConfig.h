#pragma once

#include <windows.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <shlobj.h>

#ifdef LIGHT_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

class NATIVECORE_API GameConfig {
public: // some misc things
	static HWND getWindow();

	//static std::wstring getExecutablePathW();
	//static std::string getExecutablePathA();

	static std::wstring getGameVersionW();
	static std::string getGameVersionA();

	static std::wstring getMinecraftFolderW();
	static std::string getMinecraftFolderA();
};
