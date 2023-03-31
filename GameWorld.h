#pragma once
#include <cstdint>
#include "IMemoryInterface.h"
namespace GameWorld
{

	struct BaseObject
	{
		uint64_t previousObjectLink; //0x0000
		uint64_t nextObjectLink; //0x0008
		uint64_t object; //0x0010
	};


	//intptr_t gameWorld;

	intptr_t GetGameWorld(IMemoryInterface* pMemInterface, std::array<intptr_t, 2> activeObjects);
	intptr_t GetObjectFromList(IMemoryInterface* pMemInterface, intptr_t listPointer, intptr_t lastObjectPointer, const char* objectName);
};

