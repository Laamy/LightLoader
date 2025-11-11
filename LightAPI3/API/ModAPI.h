#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <format>
#include <vector>

#ifdef LIGHT_EXPORTS
#define MODAPI_API __declspec(dllexport)
#else
#define MODAPI_API __declspec(dllimport)
#endif

class MODAPI_API ModAPI {
private:
	static std::string _err;

public:
	static bool LoadDependencyW(std::wstring dll);
	static bool LoadDependencyA(std::string dll);

	static std::vector<std::wstring> LoadDependenciesW(std::vector<std::wstring> dlls);
	static std::vector<std::string> LoadDependenciesA(std::vector<std::string> dlls);

	static std::string HasError();
	static void Error(std::string err);
};
