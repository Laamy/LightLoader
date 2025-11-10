#pragma once

void* __o__WndProc = nullptr;
LRESULT CALLBACK WndProcDetour(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CLOSE || uMsg == WM_QUIT) {// || uMsg == WM_NCDESTROY
		ClosingEvent dispatch;
		GameEvents::dispatch(&dispatch);
	}

	WndProcEvent dispatch;
	dispatch.hWnd = hWnd;
	dispatch.Msg = uMsg;
	dispatch.wParam = wParam;
	dispatch.lParam = lParam;
	GameEvents::dispatch(&dispatch);

	if (dispatch.cancel)
		return dispatch.result;

	if (dispatch.result == -1)
		return CallFunc<LRESULT, HWND, UINT, WPARAM, LPARAM>(__o__WndProc, hWnd, uMsg, wParam, lParam);
}

//anyone who makes fun of this or says u can just use setwindowshookex (no shit)
//is stupid anyways cuz they use sigs for mouse and keymap hooks which is even stupidier
void* __o__RegisterClassExW;
WORD RegisterClassExWDetour(const WNDCLASSEXW* wndCls) {
	if (wcsstr(wndCls->lpszClassName, L"Bedrock"))
		NativeCore::hookFunction(reinterpret_cast<uintptr_t>(wndCls->lpfnWndProc), reinterpret_cast<void*>(WndProcDetour), &__o__WndProc);

	return CallFunc<WORD, const WNDCLASSEXW*>(__o__RegisterClassExW, wndCls);
}

void InitModLoader() {
	NativeCore::hookFunction((uintptr_t)&RegisterClassExW, &RegisterClassExWDetour, &__o__RegisterClassExW);
}