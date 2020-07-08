#include "PackManager.h"

#include "JsonHelper.h"
#include "Globals.h"
#include "Zenova/Log.h"

namespace Zenova {
	inline std::vector<std::pair<std::string, std::string>> resource_packs;
	inline std::vector<std::pair<std::string, std::string>> behavior_packs;

    bool PackManager::addMod(const std::string& path) {
		bool result = false;
		// resource pack
		json::Document modDocument = JsonHelper::OpenFile(path + "assets\\manifest.json", false);
		if(!modDocument.IsNull()) {
			auto& rpHeaderObj = JsonHelper::FindMember(modDocument, "header");
			if(rpHeaderObj.IsObject()) {
				std::string packLocation = path.substr(path.find("mods")) + "assets";
				resource_packs.emplace_back("..\\..\\..\\" + packLocation, JsonHelper::FindString(rpHeaderObj, "uuid"));
				logger.info("{}: {}", packLocation, resource_packs.back().second);
				result = true;
			}
		}

		// behavior pack
		modDocument = JsonHelper::OpenFile(path + "data\\manifest.json", false);
		if(!modDocument.IsNull()) {
			auto& bpHeaderObj = JsonHelper::FindMember(modDocument, "header");
			if(bpHeaderObj.IsObject()) {
				std::string packLocation = path.substr(path.find("mods")) + "data";
				behavior_packs.emplace_back("..\\..\\..\\" + packLocation, JsonHelper::FindString(bpHeaderObj, "uuid"));
				logger.info("{}: {}", packLocation, behavior_packs.back().second);
				result = true;
			}
		}

        return result;
    }

	const std::vector<std::pair<std::string, std::string>>& PackManager::getResourcePacks() {
		return resource_packs;
	}

	const std::vector<std::pair<std::string, std::string>>& PackManager::getBehaviorPacks() {
		return behavior_packs;
	}
}