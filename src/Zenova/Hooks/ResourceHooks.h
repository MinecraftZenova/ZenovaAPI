#include "Zenova/Minecraft/ResourceHeaders.h"

#include "Zenova.h"
#include "Zenova/Minecraft.h"
#include "Zenova/PackManager.h"

namespace Zenova {
	static void (*_getVanillaPacks)(VanillaInPackagePacks*, std::vector<IInPackagePacks::MetaData>&, PackType);
	void getVanillaPacks(VanillaInPackagePacks* self, std::vector<IInPackagePacks::MetaData>& packs, PackType packType) {
		_getVanillaPacks(self, packs, packType);

		if (packType == PackType::Resources || packType == PackType::Behavior) {
			for (auto& pack : PackManager::getPacks(packType)) {
				packs.emplace_back(pack.path, true, PackCategory::Custom);
			}
		}
	}

	void addPacksToStack(ResourcePackRepository& repo, ResourcePackStack& tempStack, PackType type) {
		for (auto& pack : PackManager::getPacks(type)) {
			lambda::Pack lp{ repo, tempStack };
			lp.addFromUUID({ mce::UUID::fromString(pack.uuid),
								SemVersion(0, 0, 1),
								type });
		}
	}

	static void(*_initializeResourceStack)(VanillaGameModuleClient*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
	void initializeResourceStack(VanillaGameModuleClient* self, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion) {
		addPacksToStack(repo, tempStack, PackType::Resources);

		_initializeResourceStack(self, repo, tempStack, baseGameVersion);
	}

	static void(*_initializeResourceStack_1_16)(VanillaGameModuleClient*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&, GameModuleClient::ResourceLoadingPhase);
	void initializeResourceStack_1_16(VanillaGameModuleClient* self, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion, GameModuleClient::ResourceLoadingPhase loadingPhase) {
		addPacksToStack(repo, tempStack, PackType::Resources);

		_initializeResourceStack_1_16(self, repo, tempStack, baseGameVersion, loadingPhase);
	}

	static void(*_initializeResourceStack_1_16_100)(VanillaGameModuleClient*, const Experiments*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&, GameModuleClient::ResourceLoadingPhase);
	void initializeResourceStack_1_16_100(VanillaGameModuleClient* self, const Experiments* experiments, ResourcePackRepository& repo, ResourcePackStack& tempStack, const BaseGameVersion& baseGameVersion, GameModuleClient::ResourceLoadingPhase loadingPhase) {
		addPacksToStack(repo, tempStack, PackType::Resources);

		_initializeResourceStack_1_16_100(self, experiments, repo, tempStack, baseGameVersion, loadingPhase);
	}

	static void(*_initializeBehaviorStack)(VanillaGameModuleServer*, const GameRules&, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
	void initializeBehaviorStack(VanillaGameModuleServer* self, const GameRules& gameRules, ResourcePackRepository& repo, ResourcePackStack& stack, const BaseGameVersion& baseGameVersion) {
		addPacksToStack(repo, stack, PackType::Behavior);

		_initializeBehaviorStack(self, gameRules, repo, stack, baseGameVersion);
	}

	inline void createResourceHooks() {
		if (Minecraft::version() == Minecraft::v1_14_60_5) {
			Zenova_Hook(VanillaGameModuleClient::initializeResourceStack, &initializeResourceStack, &_initializeResourceStack);
		}
		else if (Minecraft::version() < Version(1, 16, 100, 4)) {
			Zenova_Hook(VanillaGameModuleClient::v1_16::initializeResourceStack, &initializeResourceStack_1_16, &_initializeResourceStack_1_16);
		}
		else {
			Zenova_Hook(VanillaGameModuleClient::v1_16_100::initializeResourceStack, &initializeResourceStack_1_16_100, &_initializeResourceStack_1_16_100);
		}

		Zenova_Hook(VanillaInPackagePacks::getPacks, &getVanillaPacks, &_getVanillaPacks);
		Zenova_Hook(VanillaGameModuleServer::initializeBehaviorStack, &initializeBehaviorStack, &_initializeBehaviorStack);
	}
}