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
#include "Zenova/Profile/Manager.h"
#include "Utils/Utils.h"

#include "Hooks/InputHooks.h"
#include "Hooks/ResourceHooks.h"

#include "initcpp.h"

namespace Zenova {
	std::string GetDataFolder() {
		std::vector<std::string> folder = {
			std::getenv("ZENOVA_DATA"),
			::Util::GetAppDirectoryA() + "\\data", //this seems to return the minecraft exe directory :/
		};

		for(auto& str : folder) {
			if(!str.empty() && Util::IsFile(str + "\\ZenovaAPI.dll")) {
				return str;
			}
		}

		return "";
	}

	u32 __stdcall Start(void* dllHandle) {
		Folder = GetDataFolder();
		bool canRun = (Platform::Init(dllHandle) && !Folder.empty());
		if(canRun) {
			MessageRedirection console;

			logger.info("Zenova Started");
			logger.info("ZenovaData Location: {}", Folder);
			logger.info("Minecraft's BaseAddress: {:x}", BaseAddress);

			manager.Init();

			logger.info("Minecraft's Version: {}", Minecraft::instance().mVersion);

			InitVersionPointers(Minecraft::instance().mVersion);

			Platform::DebugPause();
			auto getFov = Hook::SigscanCall("\xE8\x00\x00\x00\x00\x0F\x28\xF0\xF3\x44\x0F\x10\x3D\x00\x00\x00\x00", "x????xxxxxxxx????");
			logger.info("getFov: {:x}", getFov);

			createResourceHooks();
			createInputHooks();
			manager.Load(manager.GetLaunchedProfile());

			//StorageResolver storage(L"minecraftWorlds/", L"D:/MinecraftBedrock/Worlds");

			while(canRun) {
				manager.Update();
			}

			logger.info("Zenova Stopped");
		}

		Platform::Destroy();
		return 0;
	}
}