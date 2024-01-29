#pragma once

#include <string>
#include <vector>

#include "Common.h"
#include "Zenova/Profile/ModInfo.h"

#define MOD_FUNCTION extern "C" __declspec(dllexport)

struct EXPORT ModContext {
	std::string folder;
};

namespace Zenova {
	EXPORT std::vector<Zenova::ModInfo>& GetMods();
}

// Define any/all of these functions

// Called when the Mod is loaded
MOD_FUNCTION void ModLoad(ModContext& ctx);

// Called after Minecraft main
// todo: implement above (currently called after ~2 seconds)
MOD_FUNCTION void ModStart();

// Called every Minecraft tick (must be in world)
// todo: implement above (currently calls every 1/20th second)
MOD_FUNCTION void ModTick();

// Called on clean up/exit
MOD_FUNCTION void ModStop();