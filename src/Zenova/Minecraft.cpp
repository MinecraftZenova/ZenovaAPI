#include "Zenova/Minecraft.h"

#include "Zenova/Globals.h"

namespace Zenova {
	std::string Minecraft::version() {
		return manager.getVersion();
	}
}