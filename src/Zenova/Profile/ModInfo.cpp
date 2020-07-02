#include "ModInfo.h"

#include <fstream>
#include <utility>

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/PackManager.h"

namespace Zenova {
    ModInfo::ModInfo(const std::string& modName) {
        logger.info("Loading {}", modName);

        std::string folder = Folder + "\\mods\\" + modName + "\\";
        json::Document modDocument = JsonHelper::OpenFile(folder + "modinfo.json");
        if(!modDocument.IsNull()) {
            mNameId = JsonHelper::FindString(modDocument, "nameId");
            mName = JsonHelper::FindString(modDocument, "name");
            mDescription = JsonHelper::FindString(modDocument, "description");
            mVersion = JsonHelper::FindString(modDocument, "version");
            mMinVersion = JsonHelper::FindString(modDocument, "minVersion");
            mMaxVersion = JsonHelper::FindString(modDocument, "maxVersion");

            mHandle = Platform::LoadModule(folder + mNameId);
            if(mHandle) {
                using FuncPtr = Mod* (*)();
                FuncPtr createMod = reinterpret_cast<FuncPtr>(Platform::GetModuleFunction(mHandle, "CreateMod"));

                if(createMod) {
                    mMod = createMod();
                    PackManager::instance().AddMod(folder);
                }
                else {
                    logger.warn("Failed to find CreateMod in {}", mNameId);
                }
            }
            else {
                logger.warn("Failed to load {}", mName);
                Platform::ErrorPrinter();
            }
        }
    }

    ModInfo::ModInfo(ModInfo&& mod) noexcept :
        mMod(std::exchange(mod.mMod, nullptr)),
        mHandle(std::exchange(mod.mHandle, nullptr)),
        mNameId(std::move(mod.mNameId)),
        mName(std::move(mod.mName)),
        mDescription(std::move(mod.mDescription)),
        mMinVersion(std::move(mod.mMinVersion)),
        mMaxVersion(std::move(mod.mMaxVersion)),
        mVersion(std::move(mod.mVersion))
    {}

    ModInfo::~ModInfo() {
		if(mMod) mMod->Stop();
        if(mHandle) Platform::CloseModule(mHandle);
    }
}