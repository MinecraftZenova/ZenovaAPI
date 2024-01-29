#include "Zenova/Profile/ModInfo.h"

#include <fstream>
#include <utility>

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"

namespace Zenova {
    ModInfo::ModInfo(const std::string& modFolder) :
        mModFolder(modFolder)
    {
        json::Document modDocument = JsonHelper::OpenFile(mModFolder + "modinfo.json");
        if(!modDocument.IsNull()) {
            mNameId = JsonHelper::FindString(modDocument, "nameId");
            mName = JsonHelper::FindString(modDocument, "name");
            mDescription = JsonHelper::FindString(modDocument, "description");
            mVersion = JsonHelper::FindString(modDocument, "version");
        }
    }

    ModInfo::ModInfo(ModInfo&& mod) noexcept :
        mHandle(std::exchange(mod.mHandle, nullptr)),
        mModFolder(std::move(mod.mModFolder)),
        mNameId(std::move(mod.mNameId)),
        mName(std::move(mod.mName)),
        mDescription(std::move(mod.mDescription)),
        mVersion(std::move(mod.mVersion)),
        mDependencies(std::move(mod.mDependencies))
    {}

    ModInfo::~ModInfo() {
        if(mHandle) Platform::CloseModule(mHandle);
    }

    void* ModInfo::loadModule()
    {
        mHandle = Platform::LoadModModuleAndResolveImports(*this, mModFolder + mNameId);
        return mHandle;
    }
}
