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