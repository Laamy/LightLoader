#include "ModAPI.h"
#include "../Utils.h"

inline bool ModAPI::LoadDependencyW(std::wstring dll) {
    auto res = LoadLibraryW(std::format(L"mods\\{}", dll).c_str());

	if (res == NULL) {
		std::printf("Failed to load library {}, GetLastError; 0x{:#x}", dll, GetLastError());
		return false;
	}
	return true;
}

inline bool ModAPI::LoadDependencyA(std::string dll) {
	return LoadDependencyW(utf8_to_wstring(dll));
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
bool ModAPI::LoadDependenciesW(std::vector<std::wstring> dlls)
{
    size_t size = ARRAY_SIZE(dlls);

    for (size_t i = 0; i < size; ++i)
        if (!LoadDependencyW(dlls[i]))
            return false;
    return true;
}

bool ModAPI::LoadDependenciesA(std::vector<std::string> dlls)
{
    size_t size = ARRAY_SIZE(dlls);

    for (size_t i = 0; i < size; ++i)
        if (!LoadDependencyA(dlls[i]))
            return false;
    return true;
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
