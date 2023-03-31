#include "EFTPlayer.h"
#include "CameraUtilites.h"

Vector3 EFTPlayer::GetPositionFromTransform( intptr_t transform)
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

void EFTPlayer::SetEveryBone(intptr_t eftPlayer)
{
	leftKnee = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 17 * 0x8) + 0x10));
	leftAnkle = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 18 * 0x8) + 0x10));
	rightAnkle = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 23 * 0x8) + 0x10));
	rightKnee = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 22 * 0x8) + 0x10));
	middlePelvis = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 29 * 0x8) + 0x10));
	middleTorso = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 37 * 0x8) + 0x10));
	leftWrist = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 94 * 0x8) + 0x10));
	leftElbow = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 91 * 0x8) + 0x10));
	rightWrist = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 115 * 0x8) + 0x10));
	rightElbow = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 112 * 0x8) + 0x10));
	head = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 133 * 0x8) + 0x10));
	leftThigh1 = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 19 * 0x8) + 0x10));
	leftThigh2 = EFTPlayer::GetPositionFromTransform( Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 20 * 0x8) + 0x10));
	rightThigh1 = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 24 * 0x8) + 0x10));
	rightThigh2 = EFTPlayer::GetPositionFromTransform(Memory::ReadValue<intptr_t>(pMemInterface, Memory::ReadValue<intptr_t>(pMemInterface, eftPlayer + 0x20 + 25 * 0x8) + 0x10));
}

void EFTPlayer::DrawBone(intptr_t eftPlayer, Matrix matrix)
{
	Vector3 bodyPartOne = { 0,0,0 };
	Vector3 bodyPartTwo = { 0,0,0 };
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(head, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(middleTorso, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(leftElbow, matrix, bodyPartOne);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(leftWrist, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(middleTorso, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(rightElbow, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(middlePelvis, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(rightKnee, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(middlePelvis, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(leftKnee, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(leftKnee, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(leftAnkle, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(rightKnee, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(rightAnkle, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);
	CameraUtilites::worldToScreen(middleTorso, matrix, bodyPartOne);
	CameraUtilites::worldToScreen(middlePelvis, matrix, bodyPartTwo);
	DrawLine(bodyPartOne.x, bodyPartOne.y, bodyPartTwo.x, bodyPartTwo.y, BLUE);

}
