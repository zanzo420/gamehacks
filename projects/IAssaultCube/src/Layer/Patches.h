#pragma once

#include <windows.h>
#include <cstdint>
#include <tlhelp32.h>
#include <Psapi.h>
#include <iostream>
#include "Icetrix/Layer.h"
#include "Icetrix/BytePatch.h"
#include "BlackBone/Include/Types.h"

using Icetrix::Patch;

class Patches : public Icetrix::Layer
{
public:
	bool OnAttach() override;
	bool OnUpdate() override;
	void OnDetach() override;
private:
	Game g;
	std::vector<Patch>(patches) = {
		Patch{ "All weapons automatic",			0x63716, { 0x75, 0x09 }, { 0xEB, 0x09 }, 2 },
		Patch{ "Rapidfire",						0x637e4, { 0x89, 0x0A }, { 0x90, 0x90 }, 2 },
		Patch{ "No Recoil",						0x62274, { 0x0F, 0xBF, 0x8F, 0x22, 0x01, 0x00, 0x00 }, { 0x31, 0xC9, 0x90, 0x90, 0x90, 0x90, 0x90 }, 7 },
		Patch{ "No Kickback (Primary)",			0x63dd5, { 0x0F, 0xBF, 0x88, 0x16, 0x01, 0x00, 0x00 }, { 0x31, 0xc9, 0x90, 0x90, 0x90, 0x90, 0x90 }, 7 },
		Patch{ "No Kickback (Secondary)",		0x625a4, { 0x0F, 0xBF, 0x88, 0x16, 0x01, 0x00, 0x00 }, { 0x31, 0xc9, 0x90, 0x90, 0x90, 0x90, 0x90 }, 7 },
		Patch{ "No Spread",						0x620B8, { 0x0F, 0x8E, 0x88, 0x00, 0x00, 0x00 }, { 0xE9, 0x89, 0x00, 0x00, 0x00, 0x90 }, 6 },
		Patch{ "Unlimited Ammo",				0x637e9, { 0xFF, 0x0E }, { 0x90, 0x90 }, 2 },
		Patch{ "Ignore Magazine Validation",	0x6368e, { 0x75, 0x71 }, { 0xEB, 0x71 }, 2 },
		Patch{ "Unlimited Grenades",			0x63378, { 0xFF, 0x08 }, { 0x90, 0x90 }, 2 },
		Patch{ "No Grenade throw delay",		0x63370, { 0xE8, 0x2B, 0xC2, 0xFF, 0xFF }, { 0x90, 0x90, 0x90, 0x90, 0x90 }, 5 },
	};
};
