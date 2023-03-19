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

struct BaseObject
{
	uint64_t previousObjectLink; //0x0000
	uint64_t nextObjectLink; //0x0008
	uint64_t object; //0x0010
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

#define use_driver 0;

int64_t GetGameWorld(IMemoryInterface* pMemInterface) {
	intptr_t result = Memory::ReadValue<intptr_t>(pMemInterface, pMemInterface->GetModuleBase() + 0x183ABF0);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x08);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x28);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x60);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x28);
	Memory::ReadValue<int64_t>(pMemInterface, result);
	return result;
}

int64_t GetGameWorldMightNotWork(IMemoryInterface* pMemInterface,intptr_t result) {
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x30);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x18);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result+0x28);

	return result;
}

Matrix GetCameraMatrixTarkov(IMemoryInterface* pMemInterface) {
	Matrix matrix;
	intptr_t result = 0;
	result = Memory::ReadValue<intptr_t>(pMemInterface, pMemInterface->GetModuleBase() + 0x017FFD28);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x30);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x18);
	matrix = Memory::ReadValue<Matrix>(pMemInterface, result + 0x0DC);
	return matrix;
}

intptr_t GetObjectFromList(IMemoryInterface* pMemInterface,intptr_t listPointer, intptr_t lastObjectPointer, const char* objectName) {
	char name[256];
	intptr_t classNamePointer = 0x0;
	BaseObject activeObject = Memory::ReadValue<BaseObject>(pMemInterface, listPointer);
	BaseObject lastObject = Memory::ReadValue<BaseObject>(pMemInterface, lastObjectPointer);

	if (activeObject.object != 0x0) {
		while (activeObject.object != 0 && activeObject.object != lastObject.object) {
			classNamePointer = Memory::ReadValue<intptr_t>(pMemInterface,activeObject.object + 0x60);
			pMemInterface->ReadRaw(classNamePointer, &name, sizeof(name));
			if (strcmp(name, objectName) == 0) {
				return activeObject.object;
			}

			activeObject = Memory::ReadValue<BaseObject>(pMemInterface, activeObject.nextObjectLink);
		}
	}

	if (lastObject.object != 0x0) {
		classNamePointer = Memory::ReadValue<intptr_t>(pMemInterface,lastObject.object + 0x60);
		pMemInterface->ReadRaw(classNamePointer, &name, 256);
		if (strcmp(name, objectName) == 0) {
			return lastObject.object;
		}
	}

	return 0;
}

void TestOverlay::DrawImGui() {
	intptr_t gameObjectManagerDerefrenced = Memory::ReadValue<intptr_t>(pMemInterface,pMemInterface->GetModuleBase() + 0x017FFD28);
	std::array<intptr_t, 2> active_objects = Memory::ReadValue<std::array<intptr_t, 2>>(pMemInterface, gameObjectManagerDerefrenced + 0x20);
	if (!active_objects[0] || !active_objects[1]) {
		return;
	}

	intptr_t GameWorld = GetObjectFromList(pMemInterface, active_objects[1], active_objects[0], "GameWorld");
	intptr_t gameWorld = GetGameWorldMightNotWork(pMemInterface,GameWorld);
 	int64_t onlineusers = Memory::ReadValue<int64_t>(this->pMemInterface, gameWorld + 0xA0);

	if (GameWorld == gameWorld) {
		int x = 10;
	}

	if (onlineusers) {
		int player_count = Memory::ReadValue<int>(this->pMemInterface, onlineusers + 0x0018);
		ImGui::Text("PlayerCount: %i", player_count);
		int64_t list_base = Memory::ReadValue<int64_t>(this->pMemInterface, onlineusers + 0x0010);
		if (player_count > 0 && list_base) {
			constexpr auto BUFFER_SIZE = 128;
			uint64_t* player_buffer = new uint64_t[player_count];
			this->pMemInterface->ReadRaw(list_base + 0x0020, player_buffer, sizeof(uint64_t) * player_count);
			EFTPlayer player;
			intptr_t result = 0;


			for (int i = 0; i < player_count; i++) {
				player.instance = player_buffer[i];
				Vector3 screenPoint = { 0,0,0 };
				//character controller
				result = Memory::ReadValue<intptr_t>(pMemInterface, player.instance + 0x28);
				//positon (vector3)
				//0x48
				Vector3 vec3 = Memory::ReadValue<Vector3>(pMemInterface, result + 0x48);
				worldToScreen(vec3, GetCameraMatrixTarkov(pMemInterface), screenPoint);
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


