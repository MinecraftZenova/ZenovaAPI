#define _CRT_SECURE_NO_WARNINGS
#include "Helper.h"

#include <iostream> //std::cout (Zenova::MessageRedirection)
#include <algorithm> //std::find_if
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cstdlib>

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/StorageResolver.h"
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
		dataFolder = []() -> std::string {
			std::string folder[] = {
				std::getenv("ZENOVA_DATA"),
				::Util::GetAppDirectoryA() + "\\data", //this seems to return the minecraft exe directory :/
			};

			for (auto& str : folder) {
				if (!str.empty() && Util::IsFile(str + "\\ZenovaAPI.dll")) {
					return str;
				}
			}

			return "";
		}();

		bool run = (PlatformImpl::Init(platformArgs) && !dataFolder.empty());
		if(run) {
			MessageRedirection console;

			logger.info("Zenova Started");
			logger.info("ZenovaData Location: {}", dataFolder);
			logger.info("Minecraft's BaseAddress: {:x}", Platform::GetMinecraftBaseAddress());

			manager.init();

			logger.info("Minecraft's Version: {}", Minecraft::version().toString());

			InitVersionPointers(Minecraft::version());

			createResourceHooks();
			createInputHooks();

			manager.load(manager.getLaunchedProfile());

			//StorageResolver storage(L"minecraftWorlds/", L"D:/MinecraftBedrock/Worlds");

			while(run) {
				manager.update();
			}

			logger.info("Zenova Stopped");
		}

		PlatformImpl::Destroy();
		return 0;
	}
}