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
            storagePath = JsonHelper::FindString(profile, "directory", false);

            auto& mods = JsonHelper::FindMember(profile, "mods", false);
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
            }
            else {
                logger.warn("No mods found in loaded profile");
            }
        }
    }

    // verisonId is the most necessary, should we check for others?
    ProfileInfo::operator bool() const {
        return !versionId.empty();
    }
}