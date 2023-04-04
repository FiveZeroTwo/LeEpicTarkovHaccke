#pragma once
#include <raylib.h>

#include "IMemoryInterface.h"
class EFTPlayer {
public:
	EFTPlayer(IMemoryInterface* PointerMemoryInterface)
	{
		this->pMemInterface = PointerMemoryInterface;
	}

	Vector3 leftKnee;
	Vector3 leftAnkle;
	Vector3 rightAnkle;
	Vector3 rightKnee;
	Vector3 middlePelvis;
	Vector3 middleTorso;
	Vector3 leftWrist;
	Vector3 leftElbow;
	Vector3 rightWrist;
	Vector3 rightElbow;
	Vector3 head;
	Vector3 leftThigh1;
	Vector3 leftThigh2;
	Vector3 rightThigh1;
	Vector3 rightThigh2;
	uintptr_t instance;
	Vector3 location;
	intptr_t EFTPlayerClass;
	intptr_t EFTPlayerProfileClass;
	unsigned char playerSide;
	//const char playerNickName[15];


	Vector3 GetPositionFromTransform(intptr_t transform);
	void SetEveryBone(intptr_t eftPlayer);
	void DrawBone(intptr_t eftPlayer, Matrix matrix, Color color);
	intptr_t GetEFTPlayerClass();
	intptr_t GetEFTProfileClass();
	unsigned char GetPlayerSide();
	float CalculateDistance(Vector3 player, Vector3 player2);
private:
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

	IMemoryInterface* pMemInterface;
};

