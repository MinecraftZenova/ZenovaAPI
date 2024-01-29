#include "Zenova/Mod.h"
#include "Zenova/Globals.h"

EXPORT std::vector<Zenova::ModInfo>& Zenova::GetMods()
{
	return manager.getMods();
}