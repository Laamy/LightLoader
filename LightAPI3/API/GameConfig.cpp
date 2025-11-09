#include "GameConfig.h"
#include "../Utils.h"

namespace fs = std::filesystem;

HWND GameConfig::getWindow()
{
	return FindWindowW(L"Bedrock", NULL);
}

std::string GameConfig::getGameVersionA()
{
	static std::string cachedVersion;
	static std::once_flag initFlag;

	std::call_once(initFlag, [] {
		fs::path filePath = "appxmanifest.xml";
		std::ifstream file(filePath);
		std::string xml((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		std::string versionKey = "Version=\"";
		size_t start = xml.find(versionKey);

		if (start != std::string::npos) {
			start += versionKey.length();
			size_t end = xml.find("\"", start);
			if (end != std::string::npos) {
				cachedVersion = xml.substr(start, end - start);
			}
		}
	});

	return cachedVersion;
}

std::wstring GameConfig::getGameVersionW()
{
	return utf8_to_wstring(getGameVersionA());
}

std::wstring GameConfig::getMinecraftFolderW()
{
	static std::wstring cachedPath;
	static std::once_flag initFlag;

	std::call_once(initFlag, [] {
		PWSTR roaming = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &roaming))) {
			cachedPath = std::wstring(roaming) + L"\\Minecraft Bedrock";
			CoTaskMemFree(roaming);
		}
	});

	return cachedPath;
}

std::string GameConfig::getMinecraftFolderA()
{
	return wstring_to_utf8(getMinecraftFolderW());
}
