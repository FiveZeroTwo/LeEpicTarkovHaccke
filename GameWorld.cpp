#include "GameWorld.h"

intptr_t GameWorld::GetGameWorld(IMemoryInterface* pMemInterface,std::array<intptr_t,2> activeObjects)
{

	intptr_t result = GameWorld::GetObjectFromList(pMemInterface, activeObjects[1], activeObjects[0], "GameWorld");
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x30);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x18);
	result = Memory::ReadValue<intptr_t>(pMemInterface, result + 0x28);

	return result;
}

intptr_t GameWorld::GetObjectFromList(IMemoryInterface* pMemInterface, intptr_t listPointer, intptr_t lastObjectPointer, const char* objectName)
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
