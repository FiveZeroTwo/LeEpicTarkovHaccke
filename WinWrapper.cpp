#include "WinWrapper.h"
#include <Windows.h>

short WGetAsyncKeyState(int key) {
    return GetAsyncKeyState(key);
}

void WSetForegroundWindow(void* handle) {
    SetForegroundWindow((HWND)handle);
}

void WBeep(unsigned long frequency, unsigned long duration) {
    Beep(frequency, duration);
}

void WSetCurPos(int x, int y,int smoothnes) {
    INPUT i = {};
    POINT p;
    GetCursorPos(&p);
    i.type = INPUT_MOUSE;
    i.mi.dx = (x-p.x) / smoothnes;
    i.mi.dy = (y-p.y)-8 / smoothnes;
    i.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &i, sizeof(INPUT));
}