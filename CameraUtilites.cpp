#include "CameraUtilites.h"
#include "GameWorld.h"

Matrix CameraUtilites::GetCameraMatrixTarkov(IMemoryInterface* pMemInterface, std::array<intptr_t, 2> taggedObjects)
{
	Matrix matrix;
	intptr_t camera = GameWorld::GetObjectFromList(pMemInterface, taggedObjects[1], taggedObjects[0], "FPS Camera");
	camera = Memory::ReadValue<intptr_t>(pMemInterface, camera + 0x30);
	camera = Memory::ReadValue<intptr_t>(pMemInterface, camera + 0x18);
	matrix = Memory::ReadValue<Matrix>(pMemInterface, camera + 0x0DC);
	return matrix;
}

void CameraUtilites::transpose(Matrix& matrix)
{
std::swap(matrix.m1, matrix.m4);
std::swap(matrix.m2, matrix.m8);
std::swap(matrix.m3, matrix.m12);
std::swap(matrix.m6, matrix.m9);
std::swap(matrix.m7, matrix.m13);
std::swap(matrix.m11, matrix.m14);
}

bool CameraUtilites::worldToScreen(Vector3 in, Matrix matrix, Vector3& screenSpace)
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
