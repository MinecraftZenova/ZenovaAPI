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
#include "Zenova/PackManager.h"
#include "Zenova/Profile/Manager.h"
#include "Zenova/MessageRedirection.h"
#include "Utils/Utils.h"

#include "ResourceHeaders.h"
#include "initcpp.h"

namespace Zenova {
	static void(*_getVanillaPacks)(VanillaInPackagePacks*, std::vector<IInPackagePacks::MetaData>&, PackType);
	void getVanillaPacks(VanillaInPackagePacks* self, std::vector<IInPackagePacks::MetaData>& packs, PackType packType) {
		_getVanillaPacks(self, packs, packType);

		if(packType == PackType::Resources) {
			for(auto& pack : PackManager::instance.resource_packs) {
				packs.emplace_back(pack.first, true, PackCategory::Custom);
			}
		}
		else if(packType == PackType::Behavior) {
			for(auto& pack : PackManager::instance.behavior_packs) {
				packs.emplace_back(pack.first, true, PackCategory::Custom);
			}
		}
	}

	static void(*_VanillaGameModuleClient$initializeResourceStack)(VanillaGameModuleClient*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
	void VanillaGameModuleClient$initializeResourceStack(VanillaGameModuleClient* self, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion) {
		for(auto& pack : Zenova::PackManager::instance.resource_packs) {
			LambdaPack1 lp { repo, tempStack };
			addPackFromPackId(&lp,
				{ mce::UUID::fromString(pack.second), SemVersion(0, 0, 1), PackType::Resources });
		}

		_VanillaGameModuleClient$initializeResourceStack(self, repo, tempStack, baseGameVersion);
	}

	static void(*_VanillaGameModuleClient$initializeResourceStack2)(VanillaGameModuleClient*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&, GameModuleClient::ResourceLoadingPhase);
	void VanillaGameModuleClient$initializeResourceStack2(VanillaGameModuleClient* self, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion, GameModuleClient::ResourceLoadingPhase loadingPhase) {
		for(auto& pack : Zenova::PackManager::instance.resource_packs) {
			LambdaPack1 lp { repo, tempStack };
			addPackFromPackId(&lp,
				{ mce::UUID::fromString(pack.second), SemVersion(0, 0, 1), PackType::Resources });
		}

		_VanillaGameModuleClient$initializeResourceStack2(self, repo, tempStack, baseGameVersion, loadingPhase);
	}

	static void(*_VanillaGameModuleServer$initializeBehaviorStack)(VanillaGameModuleServer*, const GameRules&, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
	void VanillaGameModuleServer$initializeBehaviorStack(VanillaGameModuleServer* self, const GameRules& gameRules, ResourcePackRepository& repo, ResourcePackStack& stack, const BaseGameVersion& baseGameVersion) { 
		for(auto& pack : PackManager::instance.behavior_packs) {
			LambdaPack1 lp { repo, stack };
			addPackFromPackId(&lp,
				{ mce::UUID::fromString(pack.second), SemVersion(0, 0, 1), PackType::Behavior });
		}
		
		_VanillaGameModuleServer$initializeBehaviorStack(self, gameRules, repo, stack, baseGameVersion);
	}

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
			Manager manager;

			InitVersionPointers(manager.GetLaunchedVersion());

			if (manager.GetLaunchedVersion() == "1.14.60.5")
				Zenova::Hook::Create(&VanillaGameModuleClient::initializeResourceStack, &VanillaGameModuleClient$initializeResourceStack, &_VanillaGameModuleClient$initializeResourceStack);
			else
				Zenova::Hook::Create(&VanillaGameModuleClient::initializeResourceStack2, &VanillaGameModuleClient$initializeResourceStack2, &_VanillaGameModuleClient$initializeResourceStack2);

			Zenova::Hook::Create(&VanillaInPackagePacks::getPacks, &getVanillaPacks, &_getVanillaPacks);
			Zenova::Hook::Create(&VanillaGameModuleServer::initializeBehaviorStack, &VanillaGameModuleServer$initializeBehaviorStack, &_VanillaGameModuleServer$initializeBehaviorStack);

			logger.info("Zenova Started");
			logger.info("ZenovaData Location: {}", Folder);
			logger.info("Minecraft's BaseAddress: {:x}", BaseAddress);

			//StorageResolver storage(L"minecraftWorlds/", L"D:/MinecraftBedrock/Worlds");
			manager.Load(manager.GetLaunched());

			while(canRun) {
				manager.Update();
			}

			logger.info("Zenova Stopped");
		}

		Platform::Destroy();
		return 0;
	}
}