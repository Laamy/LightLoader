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
	HWND OverlayWnd;
	HWND TargetWnd;

	DWORD RenderThread;

	HHOOK hHook;

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

			HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);

			PWSTR pDesc = nullptr;
			bool match = false;

			if (SUCCEEDED(GetThreadDescription(hThread, &pDesc)) && pDesc) {
				if (wcsstr(pDesc, L"IO Thread") != nullptr) // this thread sets up the game grr
					match = true;
				LocalFree(pDesc);
			}

			if (!match)
				continue;

			if (suspend)
				SuspendThread((HANDLE)hThread);
			else ResumeThread((HANDLE)hThread);

			CloseHandle(hThread);
		};
	}

	CloseHandle(hSnap);
}

std::wstring status = L"Initializing";
void UpdateStatus(std::wstring wstr) {
	status = wstr;
	InvalidateRect(OvlComps::OverlayWnd, nullptr, false);
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

	UpdateStatus(L"Completed");
	Sleep(1000);

	SuspendGame(false);

	UnhookWindowsHookEx(OvlComps::hHook);
	CloseWindow(OvlComps::OverlayWnd);

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

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rcClient;
		GetClientRect(hwnd, &rcClient);

		auto bufferBitmap = Gdiplus::Bitmap((int)rcClient.right, (int)rcClient.bottom);
		Gdiplus::Graphics* gBuff = Gdiplus::Graphics::FromImage(&bufferBitmap);

		Render(gBuff, rcClient);
		delete gBuff;

		Gdiplus::Graphics gScreen(hdc);
		gScreen.DrawImage(&bufferBitmap, 0, 0, rcClient.right, rcClient.bottom);

		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SetOverlayTopmost(bool enable)
{
	SetWindowPos(
		OvlComps::OverlayWnd,
		enable ? HWND_TOPMOST : HWND_NOTOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void Refresh() {
	SetOverlayTopmost(true); // probably (overlay disappears quickly after anyways)

	RECT rcClient;
	GetClientRect(OvlComps::TargetWnd, &rcClient);

	POINT pt = { rcClient.left, rcClient.top };
	ClientToScreen(OvlComps::TargetWnd, &pt);

	RECT rcScreen = { pt.x,
					  pt.y,
					  pt.x + (rcClient.right - rcClient.left),
					  pt.y + (rcClient.bottom - rcClient.top) };

	SetWindowPos(
		OvlComps::OverlayWnd,
		HWND_NOTOPMOST,
		rcScreen.left,
		rcScreen.top,
		rcScreen.right - rcScreen.left,
		rcScreen.bottom - rcScreen.top,
		SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		const CWPSTRUCT* cw = reinterpret_cast<CWPSTRUCT*>(lParam);
		if (cw->hwnd == OvlComps::TargetWnd)
		{
			if (cw->message == WM_MOVE || cw->message == WM_SIZE ||
				cw->message == WM_SHOWWINDOW)
				Refresh();

			std::print("message {}\n", cw->message);
		}

		if (cw->message == WM_ACTIVATE || cw->message == WM_NCACTIVATE)
		{
			bool targetIsActive = (LOWORD(cw->wParam) != WA_INACTIVE);
			SetOverlayTopmost(targetIsActive);
		}

		if (cw->message == WM_PAINT) {
			std::print("MC paint event omg");

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(cw->hwnd, &ps);

			RECT rcClient;
			GetClientRect(cw->hwnd, &rcClient);

			auto bufferBitmap = Gdiplus::Bitmap((int)rcClient.right, (int)rcClient.bottom);
			Gdiplus::Graphics* gBuff = Gdiplus::Graphics::FromImage(&bufferBitmap);

			Render(gBuff, rcClient);
			delete gBuff;

			Gdiplus::Graphics gScreen(hdc);
			gScreen.DrawImage(&bufferBitmap, 0, 0, rcClient.right, rcClient.bottom);

			EndPaint(cw->hwnd, &ps);
		}
	}
	return CallNextHookEx(OvlComps::hHook, nCode, wParam, lParam);
}

int InitOverlay()
{
	LoadResources();

retry:
	Sleep(16);
	OvlComps::TargetWnd = FindWindowW(L"Bedrock", L"Minecraft");
	if (!OvlComps::TargetWnd) {
		std::wcerr << L"Could not find Minecraft window.\n";
		goto retry;
		return 1;
	}

	if (!IsWindowVisible(OvlComps::TargetWnd))
		goto retry;
	std::print("Minecraft Handle {}\n", (uintptr_t)OvlComps::TargetWnd);

	OvlComps::RenderThread = GetWindowThreadProcessId(OvlComps::TargetWnd, nullptr);
	if (!OvlComps::RenderThread)
		goto retry;

	SuspendGame(true);

	//const wchar_t* CLS_NAME = L"LightModdingOverlay";
	//WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
	//wc.lpfnWndProc = OverlayWndProc;
	//wc.hInstance = GetModuleHandle(nullptr);
	//wc.lpszClassName = CLS_NAME;
	//wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//RegisterClassEx(&wc);
	//
	//RECT rc;
	//GetClientRect(OvlComps::TargetWnd, &rc);
	//OvlComps::OverlayWnd = CreateWindowExW(
	//	WS_EX_LAYERED | WS_EX_TRANSPARENT,
	//	CLS_NAME,
	//	nullptr,
	//	WS_POPUP,
	//	0, 0,
	//	0, 0,
	//	nullptr,
	//	nullptr,
	//	GetModuleHandleW(nullptr),
	//	nullptr);
	//
	//if (!OvlComps::OverlayWnd)
	//{
	//	std::wcerr << L"Failed to create overlay window.\n";
	//	return 1;
	//}
	//
	//SetLayeredWindowAttributes(OvlComps::OverlayWnd, 0, 255, LWA_ALPHA);
	//SetLayeredWindowAttributes(
	//	OvlComps::OverlayWnd,
	//	RGB(0, 0, 0),
	//	255,
	//	LWA_COLORKEY);
	//
	//const DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
	//DwmSetWindowAttribute(OvlComps::OverlayWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

	OvlComps::hHook = SetWindowsHookExW(
		WH_CALLWNDPROC,
		CallWndProcHook,
		nullptr,
		OvlComps::RenderThread);
	if (!OvlComps::hHook)
	{
		std::wcerr << L"Failed to install hook.\n";
		return 1;
	}

	//ShowWindow(OvlComps::OverlayWnd, SW_SHOW);
	//UpdateWindow(OvlComps::OverlayWnd);

	//Refresh();

	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);
	InvalidateRect(OvlComps::TargetWnd, nullptr, false);

	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ModLoaderThread, 0, 0, 0);

	//MSG msg;
	//while (GetMessageW(&msg, nullptr, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessageW(&msg);
	//}
	//
	//UnhookWindowsHookEx(OvlComps::hHook);
	return 0;
}
