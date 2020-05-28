#include "Manager.h"

#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"

namespace Zenova {
    ProfileInfo GetLaunchedProfile() {
        json::Document prefDocument = JsonHelper::OpenFile(gFolder + "\\preferences.json");
        if(!prefDocument.IsNull()) {
            std::string profileHash = JsonHelper::FindString(prefDocument, "selectedProfile");
            if(!profileHash.empty()) {
                json::Document profilesDocument = JsonHelper::OpenFile(gFolder + "\\profiles.json");
                if(!profilesDocument.IsNull()) {
                    return JsonHelper::FindMember(profilesDocument, profileHash);
                }
            }
        }

        return ProfileInfo();
    } 

    Manager::Manager() : launched(GetLaunchedProfile()) {
        RefreshList();
        Load(launched);
    }
    
    //doesn't unload current profile
    void Manager::RefreshList() {
        profiles.clear();

        json::Document profilesDocument = JsonHelper::OpenFile(gFolder + "\\profiles.json");
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
            Zenova_Error(name + " does not exist in profile list with version: " + launched.versionId);
        }

        return ProfileInfo();
    }
    
    void Manager::Load(const ProfileInfo& profile) {
        if(!profile.name.empty()) {
            Zenova_Info("Loading " + profile.name + " profile");
            current = profile;

            for(auto& modName : profile.modNames) {
                mods.emplace_back(modName);
            }

            for(auto& modinfo : mods) {
                modinfo.mMod->Start();
            }
        }
        else {
            Zenova_Error("Empty Profile");
        }
    }

    void Manager::Swap(const ProfileInfo& profile) {
        mods.clear();

        Load(profile);
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
}