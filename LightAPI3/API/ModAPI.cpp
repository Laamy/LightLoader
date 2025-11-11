#include "ModAPI.h"
#include "../Utils.h"

inline bool ModAPI::LoadDependencyW(std::wstring dll) {
    auto res = LoadLibraryW(std::format(L"mods\\{}", dll).c_str());

    if (res != NULL) return true;

    std::wcout << std::format(L"Failed to load library {}, GetLastError; 0x{:#x}\n", dll, GetLastError());
    return false;
}

inline bool ModAPI::LoadDependencyA(std::string dll) {
	return LoadDependencyW(utf8_to_wstring(dll));
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
std::vector<std::wstring> ModAPI::LoadDependenciesW(std::vector<std::wstring> dlls)
{
    std::vector<std::wstring> result;

    for (auto dll : dlls)
        if (!LoadDependencyW(dll))
            result.push_back(dll);

    return result;
}

std::vector<std::string> ModAPI::LoadDependenciesA(std::vector<std::string> dlls)
{
    std::vector<std::string> result;

    for (auto dll : dlls)
        if (!LoadDependencyA(dll))
            result.push_back(dll);

    return result;
}

std::string ModAPI::HasError()
{
    return _err;
}

void ModAPI::Error(std::string err)
{
    _err = err;
}

std::string ModAPI::_err = "";
