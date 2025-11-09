#pragma once

#include <string>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

#ifdef LIGHT_EXPORTS
#define FILEIO_API __declspec(dllexport)
#else
#define FILEIO_API __declspec(dllimport)
#endif

class FILEIO_API FileIO {
private:
    std::wstring name;

public:
    std::wstring getMinecraftFolderW();
    std::string getMinecraftFolderA();

    bool setupClientPathW(std::wstring clientName);
    bool setupClientPathA(std::string clientName);

    std::wstring getClientPathW();
    std::string getClientPathA();

public:
    FileIO() = default;
    FileIO(std::wstring clientName) { setupClientPathW(clientName); }
    FileIO(std::string clientName) { setupClientPathA(clientName); }
};
