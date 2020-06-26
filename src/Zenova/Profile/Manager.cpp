#include "Manager.h"

#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"

namespace Zenova {
    ProfileInfo GetLaunchedProfile() {
        json::Document prefDocument = JsonHelper::OpenFile(Folder + "\\preferences.json");
        if(!prefDocument.IsNull()) {
            std::string profileHash = JsonHelper::FindString(prefDocument, "selectedProfile");
            if(!profileHash.empty()) {
                json::Document profilesDocument = JsonHelper::OpenFile(Folder + "\\profiles.json");
                if(!profilesDocument.IsNull()) {
                    return JsonHelper::FindMember(profilesDocument, profileHash);
                }
            }
        }

        return ProfileInfo();
    } 

    Manager::Manager() : launched(GetLaunchedProfile()) {
        RefreshList();
    }
    
    //doesn't unload current profile
    void Manager::RefreshList() {
        profiles.clear();

        json::Document profilesDocument = JsonHelper::OpenFile(Folder + "\\profiles.json");
        if(!profilesDocument.IsNull() && profilesDocument.IsArray()) {
            for(auto& profile : profilesDocument.GetArray()) {
                if(launched.versionId == JsonHelper::FindString(profile, "versionId")) {
                    profiles.push_back(profile);
                }
            }
        }
    }

    ProfileInfo Manager::GetProfile(const std::string& name) {
        auto profileIter = std::find_if(profiles.begin(), profiles.end(), 
            [&name](const ProfileInfo& p) { return p.name == name; });
        if(profileIter != profiles.end()) {
            return *profileIter;
        }
        else {
            logger.error("{} does not exist in profile list with version: {}", name, launched.versionId);
        }

        return ProfileInfo();
    }

    void Manager::Update() {
        for(auto& modinfo : mods) {
            modinfo.mMod->Update();
        }

        //add a 1/20th second timing
        for(auto& modinfo : mods) {
            modinfo.mMod->Tick();
        }
    }

    void Manager::Load(const ProfileInfo& profile) {
        if (!profile.name.empty()) {
            logger.info("Loading {} profile", profile.name);
            current = profile;

            mods.reserve(profile.modNames.size());
            for (auto& modName : profile.modNames) {
                mods.emplace_back(modName);
            }

            for (auto& modinfo : mods) {
                modinfo.mMod->SetManager(this);
                modinfo.mMod->Start();
            }
        }
        else {
            logger.error("Empty Profile");
        }
    }

    void Manager::Swap(const ProfileInfo& profile) {
        mods.clear();

        Load(profile);
    }

    std::string Manager::GetLaunchedVersion() const {
        return launched.versionId;
    }

    const ProfileInfo& Manager::GetLaunched() const {
        return launched;
    }
}