#pragma once

#include <numeric>

namespace offsets {
	constexpr intptr_t LocalPlayer = 0x0018AC00;
	constexpr intptr_t PlayerCount = 0x18AC0C;
	constexpr intptr_t PlayerListPtr = 0x18AC00 + 4;
	constexpr intptr_t Matrices = 0x57DF90;

	namespace player {
		constexpr intptr_t HeadPosition = 0x004;
		constexpr intptr_t FeetPosition = 0x0028;
		constexpr intptr_t ViewAngles = 0x0034;
		constexpr intptr_t Team = 0x30C;
		constexpr intptr_t Health = 0x00EC;
		constexpr intptr_t Armor = 0x00F0;
		constexpr intptr_t Name = 0x0205;
		constexpr intptr_t ActiveWeapon = 0x368;
	}
}