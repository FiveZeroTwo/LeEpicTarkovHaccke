#pragma once

short WGetAsyncKeyState(int key);
void WSetCurPos(int x, int y, int smoothnes);
void WSetForegroundWindow(void* handle);
void WBeep(unsigned long frequency, unsigned long duration);