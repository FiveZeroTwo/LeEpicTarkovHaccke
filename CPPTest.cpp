#include <iostream>
#include <Windows.h>
#include "IMemoryInterface.h"
#include "BasicMemoryInterface.h"
#include "TestOverlay.h"
#include "DriverInterop.h"

#define USE_DRIVER 1

int main() {
	IMemoryInterface* pMemoryInterface;
#if USE_DRIVER
	printf_s("Attempting to interop with driver!\n");
	DriverInterop driverInterop("TestDriver");
	pMemoryInterface = &driverInterop;
#else
	printf_s("Attempting to use basic memory interface\n");
	BasicMemoryInterface basicMemoryInterface{};
	pMemoryInterface = &basicMemoryInterface;
#endif
	TestOverlay overlay{ pMemoryInterface, "TestOverlay", -1};
	while (overlay.IsRunning()) {
		overlay.Update();
		overlay.Draw();
	}
}