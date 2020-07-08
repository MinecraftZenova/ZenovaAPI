#include "Zenova/Minecraft.h"

#include "Zenova/Globals.h"

namespace Zenova {
	const Version Minecraft::v1_14_60_5 = { 1, 14, 60, 5 };
	const Version Minecraft::v1_16_0_2 = { 1, 16, 0, 2 };
	const Version Minecraft::v1_16_1_2 = { 1, 16, 1, 2 };

	const Version& Minecraft::version() {
		static Version ver = manager.getVersion();
		return ver;
	}
}