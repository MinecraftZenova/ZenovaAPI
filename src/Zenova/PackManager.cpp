#include "PackManager.h"

#include "JsonHelper.h"
#include "Zenova/Log.h"

namespace Zenova {
	PackManager& PackManager::instance = PackManager();
    
    bool PackManager::AddMod(const std::string& path) {
		bool result = false;
		// resource pack
		std::string fileLocation = path + "assets\\manifest.json";
		std::ifstream resourceStream(fileLocation);
		if(resourceStream.is_open()) {
			json::Document modDocument;
			modDocument.ParseStream(json::IStreamWrapper(resourceStream));
			auto& headerObj = JsonHelper::FindMember(modDocument, "header");
			if(headerObj.IsObject()) {
				std::string packLocation = path.substr(path.find("mods")) + "assets";
				resource_packs.emplace_back("..\\..\\..\\" + packLocation, JsonHelper::FindString(headerObj, "uuid"));
				Zenova_Info(packLocation + ": " + resource_packs.back().second);
				result = true;
			}
		}

		// behavior pack
		fileLocation = path + "data\\manifest.json";
		std::ifstream behaviorStream(fileLocation);
		if(behaviorStream.is_open()) {
			json::Document modDocument;
			modDocument.ParseStream(json::IStreamWrapper(behaviorStream));
			auto& headerObj = JsonHelper::FindMember(modDocument, "header");
			if(headerObj.IsObject()) {
				std::string packLocation = path.substr(path.find("mods")) + "data";
				behavior_packs.emplace_back("..\\..\\..\\" + packLocation, JsonHelper::FindString(headerObj, "uuid"));
				Zenova_Info(packLocation + ": " + behavior_packs.back().second);
				result = true;
			}
		}

        return false;
    }
}