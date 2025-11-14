#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <print>
#include <filesystem>
#include <queue>
#include <fstream>
#include <tlhelp32.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

namespace fs = std::filesystem;
namespace OvlComps {
	inline bool			IsLoading = true;
	inline HWND			BedrockWnd;
	inline std::wstring	Status = L"Initializing";

	// resources for loader overlay
	inline Gdiplus::PrivateFontCollection*	pfc = nullptr;
	inline Gdiplus::Font*					LargeMojangles = nullptr;
	inline Gdiplus::Font*					SmallMojangles = nullptr;
	inline Gdiplus::Bitmap*					Bedrock = nullptr;
	inline Gdiplus::Bitmap*					McSplash = nullptr;

	inline const fs::path					mods_dir{ "mods" };
}

inline void SuspendGame(bool suspend)
{
	DWORD pid = GetCurrentProcessId();
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 te{};
	te.dwSize = sizeof(te);

	if (!Thread32First(hSnap, &te)) {
		CloseHandle(hSnap);
		return;
	}

	while (Thread32Next(hSnap, &te)) {
		if (te.th32OwnerProcessID != pid) continue;
		if (te.th32ThreadID == GetCurrentThreadId()) continue;

		HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);

		if (!hThread) continue;

		PWSTR pDesc = nullptr;
		bool match = false;
		if (SUCCEEDED(GetThreadDescription(hThread, &pDesc)) && pDesc) {
			if (wcsstr(pDesc, L"IO Thread")) match = true; // this sets up the game but not important
			LocalFree(pDesc);
		}

		if (match) {
			if (suspend)
				SuspendThread((HANDLE)hThread);
			else ResumeThread((HANDLE)hThread);
		}
	};

	CloseHandle(hSnap);
}

inline void Render(Gdiplus::Graphics* g, const RECT& rc) {
	using namespace Gdiplus;

	g->Clear(Color(239, 50, 61));

	// mojang splash
	const float titleW = OvlComps::McSplash->GetWidth() / 2.5f;
	const float titleH = OvlComps::McSplash->GetHeight() / 2.5f;
	const float x = (float)((rc.right - titleW) / 2.0f);
	g->DrawImage(OvlComps::McSplash, RectF(x, -75.0f, titleW, titleH));

	// version watermark
	SolidBrush textBrush(Color(255, 255, 255, 255));
	g->DrawString(LIGHTAPI_VERSION, -1, OvlComps::LargeMojangles,
		PointF(10, rc.bottom - 42), nullptr, &textBrush);

	// status text
	RectF bounds;
	g->MeasureString(OvlComps::Status.c_str(), -1,
		OvlComps::SmallMojangles, PointF(0, 0), &bounds);
	const float sx = (rc.right / 2.0f) - (bounds.Width / 2.0f);
	g->DrawString(OvlComps::Status.c_str(), -1, OvlComps::SmallMojangles,
		PointF(sx, 225), nullptr, &textBrush);
}

// bro i dont care if it updates or not anymore fuck it
inline void UpdateStatus(std::wstring txt) {
	OvlComps::Status = txt;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(OvlComps::BedrockWnd, &ps);
	if (!hdc) return;

	RECT rcClient;
	GetClientRect(OvlComps::BedrockWnd, &rcClient);

	Gdiplus::Bitmap bufferBitmap((int)rcClient.right, (int)rcClient.bottom);
	Gdiplus::Graphics* gBuff = Gdiplus::Graphics::FromImage(&bufferBitmap);
	Render(gBuff, rcClient);
	delete gBuff;

	Gdiplus::Graphics gScreen(hdc);
	gScreen.DrawImage(&bufferBitmap, 0, 0, rcClient.right, rcClient.bottom);

	EndPaint(OvlComps::BedrockWnd, &ps);
}

inline void ModLoaderThread() {
	using namespace OvlComps;
	if (!fs::exists(mods_dir) || !fs::is_directory(mods_dir))
		fs::create_directory(mods_dir);

	UpdateStatus(L"Loading");

	// NOTE: it is not safe to load multiple mods at the same time
	for (const auto& entry : fs::directory_iterator(mods_dir)) {
		if (entry.is_regular_file() && entry.path().extension() != ".dll")
			continue;

		const std::string modPath = entry.path().string();
		UpdateStatus(L"Loading " + entry.path().stem().wstring());
		
		HMODULE hMod = LoadLibraryA(modPath.c_str());

		auto modInfo = std::make_shared<ModInfo>();
		modInfo->Path = modPath;
		modInfo->IsLoaded = (hMod != NULL);
		modInfo->Handle = hMod;
		ModAPI::Mods.push_back(modInfo);

		const std::string err = ModAPI::hasError();
		if (!err.empty()) {
			log("[{}, ERROR] {}\n", modPath, err);
			ModAPI::error("");
			continue;
		}


		(hMod == NULL) ? log("Failed to load {} - error code {:#X}\n", modPath, GetLastError()) :
			log("Loaded {}\n", modPath);
	}

	// a mod could override the title
	char title[256]{};
	GetWindowTextA(BedrockWnd, title, sizeof(title));
	if (strcmp(title, "Minecraft") == 0)
		SetWindowTextW(BedrockWnd,
			std::format(L"Minecraft {}", GameConfig::getGameVersionW()).c_str());

	delete McSplash;
	delete Bedrock;
	delete LargeMojangles;
	delete SmallMojangles;
	delete pfc;

	SuspendGame(false);

	GameEvents::lock();

	LoadEvent dispatch;
	GameEvents::dispatch(&dispatch);
}

// before we do anything tbf
inline void LoadResources() {
	using namespace Gdiplus;
	GdiplusStartupInput gdiplusInput;
	ULONG_PTR token;
	if (GdiplusStartup(&token, &gdiplusInput, nullptr) != Ok)
		log("GDI+ failed to initialize\n");

	OvlComps::pfc = new PrivateFontCollection();
	if (OvlComps::pfc->AddFontFile(L"data\\fonts\\Mojangles.ttf") != Ok)
		log("failed to add mojangles font file\n");

	FontFamily family;
	int nFound = 0;
	OvlComps::pfc->GetFamilies(1, &family, &nFound);

	OvlComps::LargeMojangles = (nFound == 0)
		? new Font(L"Arial", 32)
		: new Font(&family, 32, FontStyleRegular, UnitPixel);
	OvlComps::SmallMojangles = (nFound == 0)
		? new Font(L"Arial", 24)
		: new Font(&family, 24, FontStyleRegular, UnitPixel);

	OvlComps::Bedrock = new Bitmap(L"data\\gui\\dist\\hbui\\assets\\bedrock-20795e0a280aa8bf0ed2.png");
	OvlComps::McSplash = new Bitmap(L"MCSplashScreen.png");
}

inline void InitMods()
{
	using namespace OvlComps;
	while (!IsWindowVisible(BedrockWnd)) {
		Sleep(0);
		BedrockWnd = FindWindowW(L"Bedrock", NULL);
	}

	SuspendGame(true);
	UpdateStatus(L"Initializing");

	CreateThread(nullptr, 0,
		reinterpret_cast<LPTHREAD_START_ROUTINE>(ModLoaderThread),
		nullptr, 0, nullptr);
}
