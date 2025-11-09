#pragma once

struct GAMEEVENTS_API WndProcEvent : public BaseEvent {
    EventID id() const override { return EventID::WndProc; }

    HWND hWnd;
    UINT Msg;
    WPARAM wParam;
    LPARAM lParam;

    LRESULT result = -1;
    bool cancel = false;
};
