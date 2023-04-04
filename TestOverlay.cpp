#include "TestOverlay.h"
#ifdef USE_IM_GUI 
#include "imgui.h"
#endif // US
#include "Offsets.h"
#include "PlayerData.h"
#include "WinWrapper.h"
#include <raylib.h>
#include "Aimbot.h"
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
		intptr_t gameObjectManagerDerefrenced = GameWorld::GetGameObjectManager(pMemInterface);
	std::array<intptr_t, 2> activeObjects = GameWorld::GetActiveObjects(pMemInterface);
	std::array<intptr_t, 2> taggedObjects = GameWorld::GetTaggedObjects(pMemInterface);
	if (!activeObjects[0] || !activeObjects[1] || !taggedObjects[0] || !taggedObjects[1]) {
			return;
	}

	intptr_t gameWorld = GameWorld::GetGameWorld(pMemInterface, activeObjects);
	int64_t onlineusers = Memory::ReadValue<int64_t>(this->pMemInterface, gameWorld + 0xA0);

	if (onlineusers) {

		int player_count = Memory::ReadValue<int>(this->pMemInterface, onlineusers + 0x0018);
#ifdef USE_IM_GUI
		ImGui::Text("PlayerCount: %i", player_count);
#endif // USE_IM_GUI

		int64_t list_base = Memory::ReadValue<int64_t>(this->pMemInterface, onlineusers + 0x0010);
		if (player_count > 0 && list_base) {
			uint64_t* player_buffer = new uint64_t[player_count];
			this->pMemInterface->ReadRaw(list_base + 0x0020, player_buffer, sizeof(uint64_t) * player_count);
			intptr_t result = 0;
			EFTPlayer localPlayer(pMemInterface);
			localPlayer.instance = player_buffer[0];
			intptr_t eftPlayer = localPlayer.GetEFTPlayerClass();
			result = Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20);
			Vector3 localPlayerVector = localPlayer.GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10));
			for (int i = 0; i < player_count; i++) {
				EFTPlayer player(pMemInterface);
				player.instance = player_buffer[i];
				Vector3 screenPoint = { 0,0,0 };
				
				intptr_t eftPlayer = player.GetEFTPlayerClass();
				result = Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20);
				player.SetEveryBone(eftPlayer);
				Vector3 vec3 = player.GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10));
				Matrix matrix = CameraUtilites::GetCameraMatrixTarkov(pMemInterface, taggedObjects);
				unsigned char playerSide = player.GetPlayerSide();
				Color color = RED;
				switch (playerSide) {
				case 4:
					color = SKYBLUE;
				break;
				case 1:
					color = YELLOW;
					break;
				}

			
				int distance = Vector3Distance(localPlayerVector, vec3);
				if (distance < 300) {
					player.DrawBone(eftPlayer, matrix,color);
				}
			
				if (!CameraUtilites::worldToScreen(vec3, matrix, screenPoint)) {
					continue;
				}

				if (WGetAsyncKeyState(0x06)) {
					Aimbot::AimAtBone(localPlayerVector, player.head, matrix, 1);
					intptr_t pwa = Memory::ReadValue<intptr_t>(pMemInterface, localPlayer.instance + 0x1A0);
					intptr_t breath = Memory::ReadValue<intptr_t>(pMemInterface, pwa + 0x28);
					intptr_t walk = Memory::ReadValue<intptr_t>(pMemInterface, pwa + 0x38);
					intptr_t shootingg = Memory::ReadValue<intptr_t>(pMemInterface, pwa + 0x48);
					Memory::Write<float>(pMemInterface, breath + 0xA4, 0);
					Memory::Write<float>(pMemInterface, shootingg + 0x74, 0);
					Memory::Write<Vector2>(pMemInterface, shootingg + 0x48, { 0,0 });
					Memory::Write<Vector2>(pMemInterface, shootingg + 0x50, { 0,0 });
					Memory::Write<float>(pMemInterface, walk + 0xD0, 0.f);
				}

				
				DrawText(TextFormat("%im", distance), screenPoint.x, screenPoint.y, 12.f, color);
#ifdef USE_IM_GUI
				if (ImGui::Selectable(TextFormat("Player %i: %f,%f,%f,%p", i, vec3.x, vec3.y, vec3.z, player.instance))) {
					std::stringstream stream{};
					stream << std::hex << player.instance;
					ImGui::SetClipboardText(stream.str().c_str());
				}
#endif // USE_IM_GUI

			}

			delete[]player_buffer;
		}
		else {
#ifdef USE_IM_GUI
			ImGui::Text("No list_base");
#endif
		}
	}
	else {
#ifdef USE_IM_GUI
		ImGui::Text("!onlineusers");
#endif
	}
}


