#include "Zenova/Minecraft/ResourceHeaders.h"

#include "Zenova.h"
#include "Zenova/Minecraft.h"
#include "Zenova/PackManager.h"

namespace Zenova {
	static void(*_getVanillaPacks)(VanillaInPackagePacks*, std::vector<IInPackagePacks::MetaData>&, PackType);
	void getVanillaPacks(VanillaInPackagePacks* self, std::vector<IInPackagePacks::MetaData>& packs, PackType packType) {
		_getVanillaPacks(self, packs, packType);

		if (packType == PackType::Resources) {
			for (auto& pack : PackManager::instance().resource_packs) {
				packs.emplace_back(pack.first, true, PackCategory::Custom);
			}
		}
		else if (packType == PackType::Behavior) {
			for (auto& pack : PackManager::instance().behavior_packs) {
				packs.emplace_back(pack.first, true, PackCategory::Custom);
			}
		}
	}

	static void(*_VanillaGameModuleClient$initializeResourceStack)(VanillaGameModuleClient*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
	void VanillaGameModuleClient$initializeResourceStack(VanillaGameModuleClient* self, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion) {
		for (auto& pack : PackManager::instance().resource_packs) {
			LambdaPack1 lp{ repo, tempStack };
			addPackFromPackId(&lp,
							  { mce::UUID::fromString(pack.second), SemVersion(0, 0, 1), PackType::Resources });
		}

		_VanillaGameModuleClient$initializeResourceStack(self, repo, tempStack, baseGameVersion);
	}

	static void(*_VanillaGameModuleClient$initializeResourceStack2)(VanillaGameModuleClient*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&, GameModuleClient::ResourceLoadingPhase);
	void VanillaGameModuleClient$initializeResourceStack2(VanillaGameModuleClient* self, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion, GameModuleClient::ResourceLoadingPhase loadingPhase) {
		for (auto& pack : PackManager::instance().resource_packs) {
			LambdaPack1 lp{ repo, tempStack };
			addPackFromPackId(&lp,
							  { mce::UUID::fromString(pack.second), SemVersion(0, 0, 1), PackType::Resources });
		}

		_VanillaGameModuleClient$initializeResourceStack2(self, repo, tempStack, baseGameVersion, loadingPhase);
	}

	static void(*_VanillaGameModuleServer$initializeBehaviorStack)(VanillaGameModuleServer*, const GameRules&, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
	void VanillaGameModuleServer$initializeBehaviorStack(VanillaGameModuleServer* self, const GameRules& gameRules, ResourcePackRepository& repo, ResourcePackStack& stack, const BaseGameVersion& baseGameVersion) {
		for (auto& pack : PackManager::instance().behavior_packs) {
			LambdaPack1 lp{ repo, stack };
			addPackFromPackId(&lp,
							  { mce::UUID::fromString(pack.second), SemVersion(0, 0, 1), PackType::Behavior });
		}

		_VanillaGameModuleServer$initializeBehaviorStack(self, gameRules, repo, stack, baseGameVersion);
	}

	inline void createResourceHooks() {
		if (Minecraft::instance().mVersion == "1.14.60.5")
			Hook::Create(&VanillaGameModuleClient::initializeResourceStack, &VanillaGameModuleClient$initializeResourceStack, &_VanillaGameModuleClient$initializeResourceStack);
		else
			Hook::Create(&VanillaGameModuleClient::initializeResourceStack2, &VanillaGameModuleClient$initializeResourceStack2, &_VanillaGameModuleClient$initializeResourceStack2);

		Hook::Create(&VanillaInPackagePacks::getPacks, &getVanillaPacks, &_getVanillaPacks);
		Hook::Create(&VanillaGameModuleServer::initializeBehaviorStack, &VanillaGameModuleServer$initializeBehaviorStack, &_VanillaGameModuleServer$initializeBehaviorStack);

	}
}