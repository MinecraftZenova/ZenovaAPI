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
#include "Zenova/MessageRedirection.h"
#include "Zenova/Minecraft.h"
#include "Zenova/Platform/PlatformImpl.h"
#include "Zenova/Profile/Manager.h"
#include "Utils/Utils.h"

#include "Hooks/InputHooks.h"
#include "Hooks/ResourceHooks.h"

#include "generated/initcpp.h"

namespace Zenova {
	u32 __stdcall start(void* platformArgs) {
		bool run = (PlatformImpl::Init(platformArgs) && !manager.dataFolder.empty());
		if(run) {
			MessageRedirection console;

			logger.info("Zenova Started");
			logger.info("ZenovaData Location: {}", manager.dataFolder);
			logger.info("Minecraft's BaseAddress: {:x}", Platform::GetMinecraftBaseAddress());

			manager.init();

			logger.info("Minecraft's Version: {}", Minecraft::version().toString());
			
			InitVersionPointers();

			createResourceHooks();
			createInputHooks();

			manager.load(manager.getLaunchedProfile());

			logger.info("Minecraft folder: {}", storage.minecraft_path);
			
			while(run) {
				manager.update();
			}

			logger.info("Zenova Stopped");
		}

		PlatformImpl::Destroy();
		return 0;
	}
}