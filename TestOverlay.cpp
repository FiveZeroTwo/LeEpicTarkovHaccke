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
#include <xmmintrin.h>  
#include <emmintrin.h>

struct EFTPlayer
{
	uintptr_t instance;
	Vector3 headPos;
	Vector3 location;
};

struct GameObjectManager
{
	uint64_t lastTaggedObject; //0x0000
	uint64_t taggedObjects; //0x0008
	uint64_t lastActiveObject; //0x0010
	uint64_t activeObjects; //0x0018
}; //Size: 0x0010

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

struct transform_access_read_only_t
{
	uint64_t transform_data{};
};

struct transform_data_t
{
	uint64_t transform_array{};
	uint64_t transform_indices{};
};

struct matrix34_t
{
	Vector4 vec0{};
	Vector4 vec1{};
	Vector4 vec2{};
};

Vector3 get_position(uintptr_t transform, IMemoryInterface* pMemInterface)
{
	__m128 result;

	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	transform_access_read_only_t pTransformAccessReadOnly = Memory::ReadValue<transform_access_read_only_t>(pMemInterface,transform + 0x38);
	unsigned int index = Memory::ReadValue<unsigned int>(pMemInterface,transform + 0x40);
	transform_data_t transformData = Memory::ReadValue<transform_data_t>(pMemInterface,pTransformAccessReadOnly.transform_data + 0x18);

	if (transformData.transform_array && transformData.transform_indices)
	{
		result = Memory::ReadValue<__m128>(pMemInterface,transformData.transform_array + (uint64_t)0x30 * index);
		int transformIndex = Memory::ReadValue<int>(pMemInterface,transformData.transform_indices + (uint64_t)0x4 * index);
		int pSafe = 0;
		while (transformIndex >= 0 && pSafe++ < 200)
		{
			matrix34_t matrix34 = Memory::ReadValue<matrix34_t>(pMemInterface,transformData.transform_array + (uint64_t)0x30 * transformIndex);

			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix34.vec0));

			transformIndex = Memory::ReadValue<int>(pMemInterface,transformData.transform_indices + (uint64_t)0x4 * transformIndex);
		}
	}

	return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}

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


intptr_t GetObjectFromList(IMemoryInterface* pMemInterface,intptr_t listPointer, intptr_t lastObjectPointer, const char* objectName) {
	char name[256];
	intptr_t classNamePointer = 0x0;
	BaseObject activeObject = Memory::ReadValue<BaseObject>(pMemInterface, listPointer);
	BaseObject lastObject = Memory::ReadValue<BaseObject>(pMemInterface, lastObjectPointer);
	if (strcmp("FPS Camera", objectName) == 0) {
		int x = 1;
	}

	if (activeObject.object != 0x0) {
 		while (activeObject.object != 0 && activeObject.object != lastObject.object) {
			classNamePointer = Memory::ReadValue<intptr_t>(pMemInterface,activeObject.object + 0x60);
			pMemInterface->ReadRaw(classNamePointer + 0x0, &name, sizeof(name));
			if (strcmp(name, objectName) == 0) {
				return activeObject.object;
			}

			activeObject = Memory::ReadValue<BaseObject>(pMemInterface, activeObject.nextObjectLink);
		}
	}

	if (lastObject.object != 0x0) {
		classNamePointer = Memory::ReadValue<intptr_t>(pMemInterface,lastObject.object + 0x60);
		pMemInterface->ReadRaw(classNamePointer+0x0, &name, 256);
		if (strcmp(name, objectName) == 0) {
			return lastObject.object;
		}
	}

	return 0;
}

Matrix GetCameraMatrixTarkov(IMemoryInterface* pMemInterface, std::array<intptr_t, 2> taggedObjects) {
	Matrix matrix;
	intptr_t camera = GetObjectFromList(pMemInterface, taggedObjects[1], taggedObjects[0], "FPS Camera");
	camera = Memory::ReadValue<intptr_t>(pMemInterface, camera + 0x30);
	camera = Memory::ReadValue<intptr_t>(pMemInterface, camera + 0x18);
	matrix = Memory::ReadValue<Matrix>(pMemInterface, camera + 0x0DC);
	return matrix;
}
void TestOverlay::DrawImGui() {
	intptr_t gameObjectManagerDerefrenced = Memory::ReadValue<intptr_t>(pMemInterface,pMemInterface->GetModuleBase() + 0x017FFD28);
	std::array<intptr_t, 2> activeObjects = Memory::ReadValue<std::array<intptr_t, 2>>(pMemInterface, gameObjectManagerDerefrenced + 0x20);
	std::array<intptr_t, 2> taggedObjects = Memory::ReadValue<std::array<intptr_t, 2>>(pMemInterface,gameObjectManagerDerefrenced + 0x8);
	if (!activeObjects[0] || !activeObjects[1] || !taggedObjects[0] || !taggedObjects[1]) {
		return;
	}

	intptr_t GameWorld = GetObjectFromList(pMemInterface, activeObjects[1], activeObjects[0], "GameWorld");
	intptr_t gameWorld = GetGameWorldMightNotWork(pMemInterface,GameWorld);
	int64_t onlineusers = Memory::ReadValue<int64_t>(this->pMemInterface, gameWorld + 0xA0);

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
				result = Memory::ReadValue<intptr_t>(pMemInterface, player.instance + 0xA8);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10);
				result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x20);
				//positon (vector3)
				//0x481cdbb5d8000
				//Vector3 vec3 = Memory::ReadValue<Vector3>(pMemInterface, result+0xAC);
				Vector3 vec3 = get_position(Memory::ReadValue<intptr_t>(pMemInterface, result + 0x10),pMemInterface);
				worldToScreen(vec3, GetCameraMatrixTarkov(pMemInterface,taggedObjects), screenPoint);
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


