#include "PackManager.h"

#include "JsonHelper.h"
#include "Globals.h"
#include "Zenova/Log.h"

namespace Zenova {
	PackManager& PackManager::instance = PackManager();
    
    bool PackManager::AddMod(const std::string& path) {
		bool result = false;
		// resource pack
		json::Document modDocument = JsonHelper::OpenFile(path + "assets\\manifest.json");
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
		modDocument = JsonHelper::OpenFile(path + "data\\manifest.json");
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
}