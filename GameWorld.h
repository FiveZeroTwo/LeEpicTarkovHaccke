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


	inline intptr_t m_gameWorld;
	inline std::array<intptr_t, 2> m_taggedObjects;
	inline std::array<intptr_t, 2> m_activeObjects;

	inline intptr_t m_gameObjectManager;

	intptr_t GetGameWorld(IMemoryInterface* pMemInterface, std::array<intptr_t, 2> activeObjects);
	std::array<intptr_t, 2> GetTaggedObjects(IMemoryInterface* pMemInterface);
	intptr_t GetGameObjectManager(IMemoryInterface* pMemInterface);
	std::array<intptr_t, 2> GetActiveObjects(IMemoryInterface* pMemInterface);
	intptr_t GetObjectFromList(IMemoryInterface* pMemInterface, intptr_t listPointer, intptr_t lastObjectPointer, const char* objectName);
};

