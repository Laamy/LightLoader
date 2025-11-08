#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <print>
#include <filesystem>
#include <queue>
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
	bool IsLoading = true;

	HWND BedrockWnd;

	// resources for loader overlay
	Gdiplus::PrivateFontCollection* pfc = nullptr;
	Gdiplus::Font* LargeMojangles = nullptr;
	Gdiplus::Font* SmallMojangles = nullptr;
	Gdiplus::Bitmap* Bedrock = nullptr;
	Gdiplus::Bitmap* McSplash = nullptr;

	// sdas
	const fs::path mods_dir = "mods";
}

void SuspendGame(bool suspend)
{
	DWORD pid = GetCurrentProcessId();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 te{};
	te.dwSize = sizeof(te);

	if (Thread32First(hSnap, &te)) {
		while (Thread32Next(hSnap, &te)) {
			if (te.th32OwnerProcessID != pid)
				continue;

			if (te.th32ThreadID == GetCurrentThreadId())
				continue;

			HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);

			PWSTR pDesc = nullptr;
			bool match = false;

			if (SUCCEEDED(GetThreadDescription(hThread, &pDesc)) && pDesc) {
				if (wcsstr(pDesc, L"IO Thread") != nullptr) // this thread sets up the game grr
					match = true;
				if (lstrlenW(pDesc) <= 3)
					match = true; // starts d3d threads & window message loop but loading libraries freezes the message loop anyways so i dont care
				LocalFree(pDesc);
			}

			if (!match)
			{
				CloseHandle(hThread);
				continue;
			}

			if (suspend)
				SuspendThread((HANDLE)hThread);
			else ResumeThread((HANDLE)hThread);

			CloseHandle(hThread);
		};
	}

	CloseHandle(hSnap);
}

void Render(Gdiplus::Graphics* g, const RECT& rcClient);

// bro i dont care if it updates or not anymore fuck it
std::wstring status = L"Initializing";
void UpdateStatus(std::wstring wstr) {
	status = wstr;

	std::print("MC paint event omg\n");
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(OvlComps::BedrockWnd, &ps);

	RECT rcClient;
	GetClientRect(OvlComps::BedrockWnd, &rcClient);

	auto bufferBitmap = Gdiplus::Bitmap((int)rcClient.right, (int)rcClient.bottom);
	Gdiplus::Graphics* gBuff = Gdiplus::Graphics::FromImage(&bufferBitmap);

	Render(gBuff, rcClient);
	delete gBuff;

	Gdiplus::Graphics gScreen(hdc);
	gScreen.DrawImage(&bufferBitmap, 0, 0, rcClient.right, rcClient.bottom);

	EndPaint(OvlComps::BedrockWnd, &ps);

	UpdateWindow(OvlComps::BedrockWnd);
}

void ModLoaderThread() {
	if (!fs::exists(OvlComps::mods_dir) || !fs::is_directory(OvlComps::mods_dir))
		fs::create_directory(OvlComps::mods_dir);

	UpdateStatus(L"Loading");

	// NOTE: it is not safe to load multiple mods at the same time
	for (const auto& entry : fs::directory_iterator(OvlComps::mods_dir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".dll") {
			auto mod = entry.path().string();

			UpdateStatus(L"Loading " + entry.path().stem().wstring());
			auto hMod = LoadLibraryA(mod.c_str());
			hMod == NULL ? std::print("Failed to load {} - error code {}\n", mod, GetLastError()) :
				std::print("Loaded {}\n", mod);
		}
	}

	SuspendGame(false);

	delete OvlComps::McSplash;
	delete OvlComps::Bedrock;
	delete OvlComps::LargeMojangles;
	delete OvlComps::SmallMojangles;
	delete OvlComps::pfc;
}

// before we do anything tbf
void LoadResources() {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
		std::print("GDI+ failed to initialize\n");

	OvlComps::pfc = new Gdiplus::PrivateFontCollection();
	if (OvlComps::pfc->AddFontFile(L"data\\fonts\\Mojangles.ttf") != Gdiplus::Ok)
		std::print("failed to add mojangles font file");
	
	Gdiplus::FontFamily family;
	int nNumFound = 0;
	OvlComps::pfc->GetFamilies(1, &family, &nNumFound);

	OvlComps::LargeMojangles = nNumFound == 0 ? new Gdiplus::Font(L"Arial", 32) : new Gdiplus::Font(&family, 32, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	OvlComps::SmallMojangles = nNumFound == 0 ? new Gdiplus::Font(L"Arial", 24) : new Gdiplus::Font(&family, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	OvlComps::Bedrock = new Gdiplus::Bitmap(L"data\\gui\\dist\\hbui\\assets\\bedrock-20795e0a280aa8bf0ed2.png");
	OvlComps::McSplash = new Gdiplus::Bitmap(L"MCSplashScreen.png");
}

void Render(Gdiplus::Graphics* g, const RECT& rcClient) {
	g->Clear(Gdiplus::Color(239, 50, 61));

	// mojangs watermark
	int titleW = OvlComps::McSplash->GetWidth() / 2.5f;
	int titleH = OvlComps::McSplash->GetHeight() / 2.5f;

	int x = (float)((rcClient.right - titleW) / 2);
	g->DrawImage(OvlComps::McSplash, Gdiplus::RectF(x, -75, titleW, titleH));

	// watermark
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
	g->DrawString(LIGHTAPI_VERSION, -1, OvlComps::LargeMojangles, Gdiplus::PointF(10, rcClient.bottom - 42), nullptr, &textBrush);

	// actual progress crap
	Gdiplus::RectF mrs;
	g->MeasureString(status.c_str(), -1, OvlComps::SmallMojangles, { 0, 0 }, &mrs);
	g->DrawString(status.c_str(), -1, OvlComps::SmallMojangles, Gdiplus::PointF((rcClient.right / 2) - (mrs.Width/2), 225), nullptr, &textBrush);
}

int InitOverlay()
{
	LoadResources();

retry:
	Sleep(16);
	OvlComps::BedrockWnd = FindWindowW(L"Bedrock", L"Minecraft");
	if (!OvlComps::BedrockWnd) {
		std::wcerr << L"Could not find Minecraft window.\n";
		goto retry;
		return 1;
	}

	if (!IsWindowVisible(OvlComps::BedrockWnd))
		goto retry;
	std::print("Minecraft Handle {}\n", (uintptr_t)OvlComps::BedrockWnd);

	SuspendGame(true);

	UpdateStatus(L"Initializing");

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ModLoaderThread, 0, 0, 0);

	//UnhookWindowsHookEx(OvlComps::hHook);
	return 0;
}
