#pragma once

inline void* __o__WndProc = nullptr;
LRESULT CALLBACK WndProcDetour(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CLOSE || uMsg == WM_QUIT) {// || uMsg == WM_NCDESTROY
		ClosingEvent dispatch;
		GameEvents::dispatch(&dispatch);
	}

	if (ImGui_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		//return true;
	}

	WndProcEvent dispatch;
	dispatch.hWnd = hWnd;
	dispatch.Msg = uMsg;
	dispatch.wParam = wParam;
	dispatch.lParam = lParam;
	GameEvents::dispatch(&dispatch);

	if (dispatch.result != -1)
		return dispatch.result;

	if (dispatch.cancel)
		return dispatch.result;

	return CallFunc<LRESULT, HWND, UINT, WPARAM, LPARAM>(__o__WndProc, hWnd, uMsg, wParam, lParam);
}

//anyone who makes fun of this or says u can just use setwindowshookex (no shit)
//is stupid anyways cuz they use sigs for mouse and keymap which is even stupidier
inline void* __o__RegisterClassExW;
WORD RegisterClassExWDetour(const WNDCLASSEXW* wndCls) {
	if (wndCls && wndCls->lpszClassName &&
		wcsstr(wndCls->lpszClassName, L"Bedrock")) {
		NativeCore::hookFunction(
			reinterpret_cast<uintptr_t>(wndCls->lpfnWndProc),
			reinterpret_cast<void*>(WndProcDetour),
			&__o__WndProc);
	}

	return CallFunc<WORD, const WNDCLASSEXW*>(__o__RegisterClassExW, wndCls);
}

inline void InitModLoader() {
	NativeCore::hookFunction(
		reinterpret_cast<uintptr_t>(&RegisterClassExW),
		reinterpret_cast<void*>(RegisterClassExWDetour),
		&__o__RegisterClassExW);
}
