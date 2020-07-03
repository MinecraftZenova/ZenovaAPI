#include "ProfileInfo.h"

#include "../Globals.h"
#include "Zenova/Log.h"

namespace Zenova {
    ProfileInfo::ProfileInfo(const json::Value& profile) {
        if(!profile.IsNull() && profile.IsObject()) {
            name = JsonHelper::FindString(profile, "name");
            created = JsonHelper::FindString(profile, "created");
            lastUsed = JsonHelper::FindString(profile, "lastUsed");
            versionId = JsonHelper::FindString(profile, "versionId");

            auto& mods = JsonHelper::FindMember(profile, "mods");
            if (!mods.IsNull()) {
                if (mods.IsArray()) {
                    for (auto& mod : mods.GetArray()) {
                        if (mod.IsString()) {
                            modNames.push_back(mod.GetString());
                        }
                    }
                }
                else if (mods.IsString()) {
                    modNames.push_back(mods.GetString());
                }
                else {
                    logger.warn("No mods found in loaded profile");
                }
            }
        }
    }

    ProfileInfo::operator bool() const {
        if (versionId.empty()) {
            return false;
        }

        return true;
    }
}