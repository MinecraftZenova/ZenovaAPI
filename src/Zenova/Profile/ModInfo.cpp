#include "ModInfo.h"

#include <fstream>

#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/PackManager.h"
#include "Zenova/Platform.h"

namespace Zenova {
    ModInfo::ModInfo(const std::string& modName) {
        Zenova_Info("Loading " + modName);

        std::string folder = gFolder + "\\mods\\" + modName + "\\";
        std::string fileLocation = folder + "modinfo.json";
        std::ifstream modInfoStream(fileLocation);
        if(modInfoStream.is_open()) {
            json::Document modDocument;
            modDocument.ParseStream(json::IStreamWrapper(modInfoStream));

            if(modDocument.IsObject()) {
                auto modinfo = modDocument.GetObject();
                
                mNameId = JsonHelper::FindString(modinfo, "nameId");
                mName = JsonHelper::FindString(modinfo, "name");
                mDescription = JsonHelper::FindString(modinfo, "description");
                mVersion = JsonHelper::FindString(modinfo, "version");
                mMinVersion = JsonHelper::FindString(modinfo, "minVersion");
                mMaxVersion = JsonHelper::FindString(modinfo, "maxVersion");

                mHandle = Platform::LoadModule(folder + mNameId);
                if(mHandle) {
                    using FuncPtr = Mod* (*)();
                    FuncPtr createMod = reinterpret_cast<FuncPtr>(Platform::GetModuleFunction(mHandle, "CreateMod"));

                    if(createMod) {
                        mMod = createMod();
                        PackManager::instance.AddMod(folder + "assets");
                    }
                    else {
                        Zenova_Warn("Failed to find CreateMod in " + mNameId);
                    }
                }
                else {
                    Zenova_Warn("Failed to load " + mName);
                    Platform::ErrorPrinter();
                }
            }
        }
        else {
            Zenova_Warn("\"" + fileLocation + "\" not found");
        }
    }

    ModInfo::~ModInfo() {
        mMod->Stop();
        Platform::CloseModule(mHandle);
    }
}