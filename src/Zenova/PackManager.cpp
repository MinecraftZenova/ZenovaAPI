#include "PackManager.h"

#include "JsonHelper.h"
#include "Globals.h"
#include "Zenova/Log.h"
#include "Zenova/Minecraft/ResourceHeaders.h"

namespace Zenova {
	using Packs = std::vector<PackManager::Pack>;

	inline Packs resource_packs;
	inline Packs behavior_packs;

	bool addToPack(Packs& packs, const std::string& path) {
		json::Document modDocument = JsonHelper::OpenFile(path + "\\manifest.json", false);
		if (!modDocument.IsNull()) {
			auto& headerObj = JsonHelper::FindMember(modDocument, "header");
			if (headerObj.IsObject()) {
				std::string packLocation = path.substr(path.find("mods"));

				// relative to the data folder in versions
				PackManager::Pack newPack = {
					"../../../" + packLocation,
					JsonHelper::FindString(headerObj, "uuid")
				};

				packs.emplace_back(newPack);
				logger.info("{}: {}", packLocation, packs.back().uuid);
				return true;
			}
		}

		return false;
	}

    bool PackManager::addMod(const std::string& path) {
		bool result = false;

		result = addToPack(resource_packs, path + "assets");
		result = addToPack(behavior_packs, path + "data");

        return result;
    }

	const Packs& PackManager::getResourcePacks() {
		return resource_packs;
	}

	const Packs& PackManager::getBehaviorPacks() {
		return behavior_packs;
	}

	const Packs& PackManager::getPacks(PackType type) {
		switch (type) {
			case PackType::Resources:
				return getResourcePacks();

			case PackType::Behavior:
				return getBehaviorPacks();

			default:
				Zenova_Error("Unknown PackType {}", type);
				return getResourcePacks();
		}
	}
}