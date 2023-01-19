#include "Helper.h"

#include <iostream> //std::cout (Zenova::MessageRedirection)
#include <algorithm> //std::find_if
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cstdlib>

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/Minecraft.h"
#include "Zenova/Platform/PlatformImpl.h"
#include "Zenova/Profile/Manager.h"
#include "Utils/Utils.h"

#include "Hooks/InputHooks.h"
#include "Hooks/ResourceHooks.h"

#include "generated/initcpp.h"

namespace Zenova {
	bool start(void* platformArgs) {
		Platform::DebugPause();

		bool run = (PlatformImpl::Init(platformArgs) && !manager.dataFolder.empty());
		if (run) {
			logger.info("Zenova Started");
			logger.info("ZenovaData Location: {}", manager.dataFolder);

			logger.info("Minecraft's Version: {}", Minecraft::version().toString());
			logger.info("Minecraft's BaseAddress: {:x}", Platform::GetMinecraftBaseAddress());
			logger.info("Minecraft's Folder: {}", Platform::GetMinecraftFolder());

			// todo: currently running into a race issue with this
			// if possible figure out how to pause the minecraft loading until everything is initialized
			// (might be fixed with separating the start into 2 functions but if possible I'd like to 
			// ensure this isn't a problem again)
			createResourceHooks();
			createInputHooks();

			manager.init();
		}
		return run;
	}

	u32 run() {
		using namespace std::chrono_literals;

		// a bit hacky but wait for static variables to be initialized
		// todo: hook into minecraft's main and signal this thread to unblock?
		std::this_thread::sleep_for(2s);

		manager.run();
		
		while (true) {
			manager.update();
		}

		logger.info("Zenova Stopped");

		PlatformImpl::Destroy();
		return 0;
	}
}