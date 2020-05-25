#include "PackManager.h"

#include "JsonHelper.h"
#include "Zenova/Log.h"

namespace Zenova {
	PackManager& PackManager::instance = PackManager();
    
    bool PackManager::AddMod(const std::string& path) {
        std::string fileLocation = path + "\\manifest.json";
        std::ifstream profilesStream(fileLocation);
        if(profilesStream.is_open()) {
            json::Document modDocument;
            modDocument.ParseStream(json::IStreamWrapper(profilesStream));
            auto& headerObj = JsonHelper::FindMember(modDocument, "header");
            if(headerObj.IsObject()) {
                resource_packs.emplace_back("..\\..\\..\\" + path.substr(path.find("mods")), JsonHelper::FindString(headerObj, "uuid"));
                Zenova_Info(resource_packs.back().first + " (" + resource_packs.back().second + ")");
                return true;
            }
        }

        return false;
    }
}