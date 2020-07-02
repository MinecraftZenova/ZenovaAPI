#include "Zenova/Minecraft.h"

#include "Zenova/Globals.h"

namespace Zenova {
	const Minecraft& Minecraft::instance() {
		static Minecraft singleton;
		return singleton;
	}

	//force it to be initialized at the beginning 
	auto init = Minecraft::instance();

	Minecraft::Minecraft() {
		manager.setVersionCallback([this](const std::string& version) { mVersion = version; });
	}
}