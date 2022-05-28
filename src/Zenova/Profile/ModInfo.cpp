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

        std::string folder = manager.dataFolder + "\\mods\\" + modName + "\\";
        json::Document modDocument = JsonHelper::OpenFile(folder + "modinfo.json");
        if(!modDocument.IsNull()) {
            mNameId = JsonHelper::FindString(modDocument, "nameId");
            mName = JsonHelper::FindString(modDocument, "name");
            mDescription = JsonHelper::FindString(modDocument, "description");
            mVersion = JsonHelper::FindString(modDocument, "version");

            auto& objArray = JsonHelper::FindMember(modDocument, "mcversion");
            if (!objArray.IsNull()) {
                if (objArray.IsString()) {
                    mMcVersion = { objArray.GetString() };
                }
                else if (objArray.IsArray()) {
                    for (auto& ver : objArray.GetArray()) {
                        if (ver.IsString()) {
                            mMcVersion.push_back(ver.GetString());
                        }
                    }
                }
                else {
                    logger.warn("No version found in {}", modName);
                }
            }

            mHandle = Platform::LoadModule(folder + mNameId);
            if(mHandle) {
                using FuncPtr = Mod* (*)();
                FuncPtr createMod = reinterpret_cast<FuncPtr>(Platform::GetModuleFunction(mHandle, "CreateMod"));

                if(createMod) {
                    mMod = createMod();
                    PackManager::addMod(folder);
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
        mVersion(std::move(mod.mVersion)),
        mMcVersion(std::move(mMcVersion))
    {}

    ModInfo::~ModInfo() {
		if(mMod) mMod->Stop();
        if(mHandle) Platform::CloseModule(mHandle);
    }
}