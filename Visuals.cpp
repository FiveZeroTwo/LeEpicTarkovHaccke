#include "Visuals.h"
#include <raylib.h>
#include <raymath.h>


void Visuals::DrawBone(IMemoryInterface* pMemInterface, EFTPlayer player, Matrix matrix)
{
	Vector3 bodyPartOne = { 0,0,0 };
	Vector3 bodyPartTwo = { 0,0,0 };


	worldToScreen(player.leftAnkle, matrix, bodyPartOne);
	worldToScreen(player.leftKnee, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.middlePelvis, matrix, bodyPartOne);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.rightAnkle, matrix, bodyPartOne);
	worldToScreen(player.rightKnee, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.middlePelvis, matrix, bodyPartOne);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.head, matrix, bodyPartOne);
	worldToScreen(player.middleTorso, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.leftElbow, matrix, bodyPartOne);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.leftWrist, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.middleTorso, matrix, bodyPartOne);
	worldToScreen(player.rightElbow, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	worldToScreen(player.middlePelvis, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
}

Matrix Visuals::GetCameraMatrixTarkov(IMemoryInterface* pMemInterface, std::array<intptr_t, 2> taggedObjects)
{
	Matrix matrix;
	intptr_t camera = GetObjectFromList(pMemInterface, taggedObjects[1], taggedObjects[0], "FPS Camera");
	camera = Memory::ReadValue<intptr_t>(pMemInterface, camera + 0x30);
	camera = Memory::ReadValue<intptr_t>(pMemInterface, camera + 0x18);
	matrix = Memory::ReadValue<Matrix>(pMemInterface, camera + 0x0DC);
	return matrix;
}

intptr_t Visuals::GetObjectFromList(IMemoryInterface* pMemInterface, intptr_t listPointer, intptr_t lastObjectPointer, const char* objectName)
{
	char name[256];
	intptr_t classNamePointer = 0x0;
	BaseObject activeObject = Memory::ReadValue<BaseObject>(pMemInterface, listPointer);
	BaseObject lastObject = Memory::ReadValue<BaseObject>(pMemInterface, lastObjectPointer);
	if (strcmp("FPS Camera", objectName) == 0) {
		int x = 1;
	}

	if (activeObject.object != 0x0) {
		while (activeObject.object != 0 && activeObject.object != lastObject.object) {
			classNamePointer = Memory::ReadValue<intptr_t>(pMemInterface, activeObject.object + 0x60);
			pMemInterface->ReadRaw(classNamePointer + 0x0, &name, sizeof(name));
			if (strcmp(name, objectName) == 0) {
				return activeObject.object;
			}

			activeObject = Memory::ReadValue<BaseObject>(pMemInterface, activeObject.nextObjectLink);
		}
	}

	if (lastObject.object != 0x0) {
		classNamePointer = Memory::ReadValue<intptr_t>(pMemInterface, lastObject.object + 0x60);
		pMemInterface->ReadRaw(classNamePointer + 0x0, &name, 256);
		if (strcmp(name, objectName) == 0) {
			return lastObject.object;
		}
	}

	return 0;
}

int64_t Visuals::GetGameWorld(IMemoryInterface* pMemInterface, intptr_t result)
{
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x30);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x18);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);

	return result;
}

Vector3 Visuals::GetPosition(IMemoryInterface* pMemInterface, uintptr_t transform)
{
	__m128 result;

	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	transform_access_read_only_t pTransformAccessReadOnly = Memory::ReadValue<transform_access_read_only_t>(pMemInterface, transform + 0x38);
	unsigned int index = Memory::ReadValue<unsigned int>(pMemInterface, transform + 0x40);
	transform_data_t transformData = Memory::ReadValue<transform_data_t>(pMemInterface, pTransformAccessReadOnly.transform_data + 0x18);

	if (transformData.transform_array && transformData.transform_indices)
	{
		result = Memory::ReadValue<__m128>(pMemInterface, transformData.transform_array + (uint64_t)0x30 * index);
		int transformIndex = Memory::ReadValue<int>(pMemInterface, transformData.transform_indices + (uint64_t)0x4 * index);
		int pSafe = 0;
		while (transformIndex >= 0 && pSafe++ < 200)
		{
			matrix34_t matrix34 = Memory::ReadValue<matrix34_t>(pMemInterface, transformData.transform_array + (uint64_t)0x30 * transformIndex);

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

			transformIndex = Memory::ReadValue<int>(pMemInterface, transformData.transform_indices + (uint64_t)0x4 * transformIndex);
		}
	}

	return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}

bool Visuals::worldToScreen(Vector3 in, Matrix matrix, Vector3& screenSpace)
{
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

void Visuals::transpose(Matrix& matrix)
{
}
