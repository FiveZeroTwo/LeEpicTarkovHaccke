#include "Overlay.h"
#include <raylib.h>
#ifdef USE_IM_GUI
#include "rlImGui.h"
#endif
#include "WinWrapper.h"

#include <iostream>

static Color BACKGROUND{ 0, 0, 0, 0 };

Overlay::Overlay(const char* title, int updateRate) : title(title), isMenuOpen(false) {
	SetConfigFlags(
		ConfigFlags::FLAG_MSAA_4X_HINT |
		ConfigFlags::FLAG_WINDOW_UNDECORATED |
		ConfigFlags::FLAG_WINDOW_TOPMOST |
		ConfigFlags::FLAG_WINDOW_TRANSPARENT |
		ConfigFlags::FLAG_WINDOW_UNFOCUSED |
		ConfigFlags::FLAG_WINDOW_MOUSE_PASSTHROUGH
	);

	int length = GetRandomValue(16, 48);
	std::string overlayTitle{};
	overlayTitle.reserve(GetRandomValue(16, 48));
	for (int i = 0; i < length; i++) {
		overlayTitle.append(1,
			GetRandomValue(0, 1) == 0 ?
				GetRandomValue('a', 'z') :
				GetRandomValue('A', 'Z')
		);
	}

	InitWindow(0, 0, overlayTitle.c_str());
	SetWindowSize(GetScreenWidth(), GetScreenHeight() - 1);
	if (updateRate != 0) {
		SetTargetFPS(updateRate < 0 ? GetMonitorRefreshRate(GetCurrentMonitor()) : updateRate);
	}
#ifdef USE_IM_GUI
	rlImGuiSetup(true);

#endif // USE_IM_GUI

}

Overlay::~Overlay() {
#ifdef USE_IM_GUI
	rlImGuiShutdown();
#endif // USE_IM_GUI

	CloseWindow();
}

void Overlay::Update() {
	if (
		IsWindowFocused() ?
		IsKeyPressed(KeyboardKey::KEY_HOME) :
		WGetAsyncKeyState(0x24) & 1
		)
	{
		if ((this->isMenuOpen = !this->isMenuOpen) == true) {
			ClearWindowState(ConfigFlags::FLAG_WINDOW_MOUSE_PASSTHROUGH);
			WSetForegroundWindow(GetWindowHandle());
			this->OnFocusFound();
		}
		else {
			SetWindowState(ConfigFlags::FLAG_WINDOW_MOUSE_PASSTHROUGH);
			this->OnFocusLost();
		}
	}

	this->UpdateImGui();
}

void Overlay::Draw() {
	BeginDrawing();
	ClearBackground(BACKGROUND);
#ifdef USE_IM_GUI


	rlImGuiBegin();
	bool shouldShow = this->ShouldShowMenu();
	bool needsEnd = false;
	if (shouldShow) {
		needsEnd = ImGui::Begin(
			this->title,
			nullptr,
			ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse
		);
	}
#endif // USE_IM_GUI

	this->DrawImGui();
#ifdef USE_IM_GUI
	if (needsEnd) {
		ImGui::End();
	}

	rlImGuiEnd();
#endif

	EndDrawing();
}

bool Overlay::IsRunning() {
	return !WindowShouldClose();
}
