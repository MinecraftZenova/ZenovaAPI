#include "ModInfo.h"

#include <fstream>
#include <filesystem>
#include <utility>

#include "Zenova.h"
#include "Zenova/JsonHelper.h"

#include "Zenova/Profile/Manager.h"
#include <Zenova/Mod.h>

namespace Zenova {
    ModInfo::ModInfo(const std::string& modFolder, const std::string& nameId) : mNameId(nameId) {
        folder = modFolder;
        
        std::string file = modFolder + "modinfo.json";
        if (std::filesystem::exists(file)) {
            json::Document modDocument = JsonHelper::OpenFile(file);
            if (!modDocument.IsNull()) {
                mNameId = JsonHelper::FindString(modDocument, "nameId");
                mName = JsonHelper::FindString(modDocument, "name");
                mDescription = JsonHelper::FindString(modDocument, "description");
                mVersion = JsonHelper::FindString(modDocument, "version");
            }
        }

        if (!mNameId.empty()) {
            mHandle = Platform::LoadModule(modFolder + mNameId);
        }
    }

    ModInfo::~ModInfo() {
        if(mHandle) Platform::CloseModule(mHandle);
    }

    std::deque<ModContext>& GetMods() {
        return manager.getMods();
    }

    void* ModContext::GetHandle() {
        return info->mHandle;
    }

    std::string ModContext::GetFolder() {
        return info->folder;
    }
}
