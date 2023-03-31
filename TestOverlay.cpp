#include "TestOverlay.h"
#include "imgui.h"
#include "Offsets.h"
#include "PlayerData.h"
#include "WinWrapper.h"
#include <raylib.h>
#include <string>
#include "EFTPlayer.h"
#include <iostream>
#include <raymath.h>
#include <sstream>
#include <vector>
#include <xmmintrin.h>  
#include "GameWorld.h"
#include <emmintrin.h>
#include "CameraUtilites.h"



TestOverlay::TestOverlay(IMemoryInterface* pMemInterface, const char* title, int updateRate) : Overlay(title, updateRate), pMemInterface(pMemInterface) {
}

void TestOverlay::OnFocusFound() {
}

void TestOverlay::OnFocusLost() {
}

void TestOverlay::UpdateImGui() {
	while (!this->pMemInterface->UpdateProcessId(L"EscapeFromTarkov.exe")) {
	}
}

bool TestOverlay::ShouldShowMenu() {
	return true;
}






void TestOverlay::DrawImGui() {
	intptr_t gameObjectManagerDerefrenced = Memory::ReadValue<intptr_t>(pMemInterface, pMemInterface->GetModuleBase() + 0x017FFD28);
	std::array<intptr_t, 2> activeObjects = Memory::ReadValue<std::array<intptr_t, 2>>(pMemInterface, gameObjectManagerDerefrenced + 0x20);
	std::array<intptr_t, 2> taggedObjects = Memory::ReadValue<std::array<intptr_t, 2>>(pMemInterface, gameObjectManagerDerefrenced + 0x8);
	if (!activeObjects[0] || !activeObjects[1] || !taggedObjects[0] || !taggedObjects[1]) {
			return;
	}

	intptr_t gameWorld = GameWorld::GetGameWorld(pMemInterface, activeObjects);
	int64_t onlineusers = Memory::ReadValue<int64_t>(this->pMemInterface, gameWorld + 0xA0);

	if (onlineusers) {

		int player_count = Memory::ReadValue<int>(this->pMemInterface, onlineusers + 0x0018);
		ImGui::Text("PlayerCount: %i", player_count);
		int64_t list_base = Memory::ReadValue<int64_t>(this->pMemInterface, onlineusers + 0x0010);
		if (player_count > 0 && list_base) {
			constexpr auto BUFFER_SIZE = 128;
			uint64_t* player_buffer = new uint64_t[player_count];
			this->pMemInterface->ReadRaw(list_base + 0x0020, player_buffer, sizeof(uint64_t) * player_count);
			intptr_t result = 0;


			for (int i = 1; i < player_count; i++) {
				EFTPlayer player(pMemInterface);
				player.instance = player_buffer[i];
				Vector3 screenPoint = { 0,0,0 };
				result = Memory::ReadValue<intptr_t>(pMemInterface, player.instance + 0xA8);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);
				intptr_t eftPlayer = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10);
				result = Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20);
				player.SetEveryBone(eftPlayer);

				Vector3 vec3 = player.GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10));
				Matrix matrix = CameraUtilites::GetCameraMatrixTarkov(pMemInterface, taggedObjects);
				player.DrawBone(eftPlayer, matrix);
				CameraUtilites::worldToScreen(vec3, matrix, screenPoint);
				DrawText("Person", screenPoint.x, screenPoint.y, 12.f, BLUE);
				if (ImGui::Selectable(TextFormat("Player %i: %f,%f,%f,%p", i, vec3.x, vec3.y, vec3.z, player.instance))) {
					std::stringstream stream{};
					stream << std::hex << player.instance;
					ImGui::SetClipboardText(stream.str().c_str());
				}
			}

			delete[]player_buffer;
		}
		else {
			ImGui::Text("No list_base");
		}
	}
	else {
		ImGui::Text("!onlineusers");
	}
}


