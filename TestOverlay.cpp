#include "TestOverlay.h"
#include "imgui.h"
#include "Offsets.h"
#include "PlayerData.h"
#include "WinWrapper.h"
#include <raylib.h>
#include <string>
#include <iostream>
#include <raymath.h>
#include <sstream>
#include <vector>


struct EFTPlayer
{
	uintptr_t instance;
	Vector3 headPos;
	Vector3 location;
};

class ListInternal
{
public:
	char pad_0x0000[0x20]; //0x0000
	uintptr_t firstEntry; //0x0020 
}; //Size=0x0028

class List
{
public:
	char padding[10];
	ListInternal* listBase; //0x0010 
	__int32 itemCount; //0x0018 
}; //Size=0x001C

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

void transpose(Matrix& matrix) {
	std::swap(matrix.m1, matrix.m4);
	std::swap(matrix.m2, matrix.m8);
	std::swap(matrix.m3, matrix.m12);
	std::swap(matrix.m6, matrix.m9);
	std::swap(matrix.m7, matrix.m13);
	std::swap(matrix.m11, matrix.m14);
}

bool worldToScreen(Vector3 in, Matrix matrix, Vector3& screenSpace) {
	transpose(matrix);
          	Vector3 translationVector = Vector3{ matrix.m3,matrix.m7,matrix.m11 };
	Vector3 up = Vector3{ matrix.m1,matrix.m5,matrix.m9 };
	Vector3 right = Vector3{ matrix.m0,matrix.m4,matrix.m8 };

	float w = Vector3DotProduct(translationVector, in) + matrix.m15;

	if (w < 0.098f)
		return false;

	float y = Vector3DotProduct(in, up) + matrix.m13;
	float x = Vector3DotProduct(in, right) + matrix.m12;
	screenSpace.x = (1920 / 2) * (1.f + x / w);
	screenSpace.y = (1080 / 2) * (1.f - y / w);
	screenSpace.z = w;

	return true;
}

intptr_t GetGameWorld(IMemoryInterface* pMemInterface) {
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, pMemInterface->GetModuleBase() + 0x8);
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, result);
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x28);
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, result);
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x60);
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x28);
	return Memory::ReadValue<intptr_t>(pMemInterface, result);
	//niggerbullshuitmonek
}

void TestOverlay::DrawImGui() {
	if (1) {
		int64_t onlineusers = Memory::ReadValue<int64_t>(this->pMemInterface, GetGameWorld(pMemInterface) + 0xA0);
		
		if (onlineusers) {
			int player_count = Memory::ReadValue<int>(this->pMemInterface, onlineusers + 0x0018);
			ImGui::Text("PlayerCount: %i", player_count);
			int64_t list_base = Memory::ReadValue<int64_t>(this->pMemInterface, onlineusers + 0x0010);
			if (player_count > 0 && list_base) {
				constexpr auto BUFFER_SIZE = 128;
				uint64_t* player_buffer = new uint64_t[player_count];
				this->pMemInterface->ReadRaw(list_base + 0x0020, player_buffer, sizeof(uint64_t) * player_count);
				EFTPlayer player;
				Matrix matrix;
				intptr_t result = 0;
				transpose(matrix);

				result = Memory::ReadValue<intptr_t>(pMemInterface, pMemInterface->GetModuleBase() + 0x017FFD28);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x30);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x18);
				matrix = Memory::ReadValue<Matrix>(pMemInterface, result + 0x0DC);

				for (int i = 0; i < player_count; i++) {
					player.instance = player_buffer[i];
					Vector3 screenPoint = { 0,0,0};
					result = Memory::ReadValue<intptr_t>(pMemInterface, player.instance + 0x28);
					Vector3 vec3 = Memory::ReadValue<Vector3>(pMemInterface, result + 0xAC);
					worldToScreen(vec3, matrix, screenPoint);
					DrawText("Person", screenPoint.x, screenPoint.y, 12.f,BLUE);
					if (ImGui::Selectable(TextFormat("Player %i: %f,%f,%f,%p", i, vec3.x,vec3.y,vec3.z,player.instance))) {
						std::stringstream stream{};
						stream << std::hex << player.instance;
						ImGui::SetClipboardText(stream.str().c_str());
					}
				}
			}
			else {
				ImGui::Text("No list_base");
			}
		}
		else {
			ImGui::Text("!onlineusers");
		}
	}
}


