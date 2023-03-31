#pragma once
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include "IMemoryInterface.h"
#include <cstdint>
#include <array>
namespace CameraUtilites
{
	
	Matrix GetCameraMatrixTarkov(IMemoryInterface* pMemInterface, std::array<intptr_t, 2> taggedObjects);
	void transpose(Matrix& matrix);
	bool worldToScreen(Vector3 in, Matrix matrix, Vector3& screenSpace);
}

