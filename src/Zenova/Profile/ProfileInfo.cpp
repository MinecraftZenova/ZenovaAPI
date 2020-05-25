#include "ProfileInfo.h"

#include "Zenova/Log.h"

namespace Zenova {
    ProfileInfo::ProfileInfo(const json::Value& profile) {
        if(profile.IsObject()) {
            name = JsonHelper::FindString(profile, "name");
            created = JsonHelper::FindString(profile, "created");
            lastUsed = JsonHelper::FindString(profile, "lastUsed");
            versionId = JsonHelper::FindString(profile, "versionId");

            auto& mods = JsonHelper::FindMember(profile, "mods");
            if(mods.IsArray()) {
                for(auto& mod : mods.GetArray()) {
                    if(mod.IsString()) {
                        modNames.push_back(mod.GetString());
                    }
                }
            }
            else if(mods.IsString()) {
                modNames.push_back(mods.GetString());
            }
            else {
                Zenova_Warn("No mods found in loaded profile");
            }
        }
    }
}