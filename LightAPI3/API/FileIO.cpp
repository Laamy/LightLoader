#include "FileIO.h"
#include "../Utils.h"

#include "GameConfig.h"

std::wstring FileIO::getMinecraftFolderW()
{
    return GameConfig::getMinecraftFolderW();
}

std::string FileIO::getMinecraftFolderA()
{
    std::wstring wpath = getMinecraftFolderW();
    return wstring_to_utf8(wpath);
}

#include <iostream>
bool FileIO::setupClientPathW(std::wstring clientName)
{
    if (clientName.empty())
        throw std::invalid_argument("Client name is empty, please provide a valid name.");

    name = clientName;

    if (!fs::exists(getClientPathW()))
        return fs::create_directory(getClientPathW());

    return false;
}

bool FileIO::setupClientPathA(std::string clientName)
{
    return setupClientPathW(utf8_to_wstring(clientName));
}

std::wstring FileIO::getClientPathW()
{
    if (name.empty())
        throw std::invalid_argument("Client name is not set, please call setupClientPath() first.");

    return getMinecraftFolderW() + L"\\mods\\" + name + L"\\";
}

std::string FileIO::getClientPathA()
{
    return wstring_to_utf8(getClientPathW());
}

IniBuilder FileIO::getIniStream(std::string path)
{
    return IniBuilder(this->getClientPathA(), path);
}
