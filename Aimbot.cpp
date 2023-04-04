#include "Aimbot.h"
#include "CameraUtilites.h"
#include "WinWrapper.h"
void Aimbot::AimAtBone(Vector3 localPlayerPosition,Vector3 Bone,Matrix matrix,float smoothnes)
{
	Vector3 vec3;
	if (!CameraUtilites::worldToScreen(Bone, matrix, vec3)) {
		return;
	}
	auto w = Vector3Distance({ 1920 / 2,1080 / 2 }, vec3);
	if (Vector3Distance(localPlayerPosition, Bone) < 100 && w < 55) {
		WSetCurPos(vec3.x, vec3.y, smoothnes);
	}
}
