#include "Zenova/Mod.h"

#include "Zenova/Globals.h"

namespace Zenova {
	// todo: get the actual mod folder, for the time being pass the zenova folder
	Mod::Mod() {
		ctx.folder = manager.dataFolder;
	}
}