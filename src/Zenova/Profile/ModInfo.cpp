#include "ModInfo.h"

#include <fstream>
#include <utility>

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"

namespace Zenova {
    ModInfo::ModInfo(const std::string& modFolder) {
        json::Document modDocument = JsonHelper::OpenFile(modFolder + "modinfo.json");
        if(!modDocument.IsNull()) {
            mNameId = JsonHelper::FindString(modDocument, "nameId");
            mName = JsonHelper::FindString(modDocument, "name");
            mDescription = JsonHelper::FindString(modDocument, "description");
            mVersion = JsonHelper::FindString(modDocument, "version");

            mHandle = Platform::LoadModule(modFolder + mNameId);
        }
    }

    ModInfo::ModInfo(ModInfo&& mod) noexcept :
        mHandle(std::exchange(mod.mHandle, nullptr)),
        mNameId(std::move(mod.mNameId)),
        mName(std::move(mod.mName)),
        mDescription(std::move(mod.mDescription)),
        mVersion(std::move(mod.mVersion))
    {}

    ModInfo::~ModInfo() {
        if(mHandle) Platform::CloseModule(mHandle);
    }
}