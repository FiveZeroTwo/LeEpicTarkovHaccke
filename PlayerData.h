#pragma once

#include "IMemoryInterface.h"
#include "Offsets.h"
#include <numeric>
#include <raylib.h>
#include <iostream>
#include <string>

class PlayerData {
public:
	PlayerData(IMemoryInterface* pMemoryInterface) : pMemoryInterface(pMemoryInterface), baseAddress(0) {
	}

	Vector3 GetPosition(bool headPosition) {
		if (!this->baseAddress) {
			return Vector3{ 0.f, 0.f, 0.f };
		}

		return Memory::ReadValue<Vector3>(this->pMemoryInterface, this->baseAddress + (headPosition ? offsets::player::HeadPosition : offsets::player::FeetPosition));
	}

	bool SetPosition(const Vector3& position) {
		if (!this->baseAddress) {
			return false;
		}

		return Memory::Write<Vector3>(this->pMemoryInterface, this->baseAddress + offsets::player::FeetPosition, position);
	}

	Vector2 GetViewAngles() {
		if (!this->baseAddress) {
			return Vector2{ 0.f, 0.f };
		}

		return Memory::ReadValue<Vector2>(this->pMemoryInterface, this->baseAddress + offsets::player::ViewAngles);
	}

	bool SetViewAngles(const Vector2& viewAngles) {
		if (!this->baseAddress) {
			return false;
		}

		return Memory::Write<Vector2>(this->pMemoryInterface, this->baseAddress + offsets::player::ViewAngles, viewAngles);
	}

	const char* GetName() {
		char* buffer = new char[16];
		rsize_t length = 0;
		for (int i = 0; i < 16; i++) {
			const char readChar = Memory::ReadValue<char>(this->pMemoryInterface, this->baseAddress + offsets::player::Name + i);
			length++;
			buffer[i] = readChar;
			if (readChar == '\0') {
				break;
			}
		}

		return buffer;
	}

	int GetTeam() {
		return Memory::ReadValue<int>(this->pMemoryInterface, this->baseAddress + offsets::player::Team);
	}

	int GetHealth() {
		return Memory::ReadValue<int>(this->pMemoryInterface, this->baseAddress + offsets::player::Health);
	}

	int GetArmor() {
		return Memory::ReadValue<int>(this->pMemoryInterface, this->baseAddress + offsets::player::Armor);
	}

	intptr_t baseAddress;
private:
	IMemoryInterface* pMemoryInterface;
};