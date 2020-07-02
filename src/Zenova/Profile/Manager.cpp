#include "Manager.h"

#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"

namespace Zenova {
    Manager::Manager() {}

    void Manager::Init() {
        launched = GetLaunchedProfile();

        if (updateVersion)
            updateVersion(launched.versionId);

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

        //I should probably hook into minecraft's global tick function
        namespace chrono = std::chrono;
        using tick = chrono::duration<int, std::ratio<1, 20>>;

        if (chrono::duration_cast<tick>(clock::now() - tickTimer).count() >= 1) {
            tickTimer = clock::now();

            for (auto& modinfo : mods) {
                modinfo.mMod->Tick();
            }
        }
    }

    void Manager::Load(const ProfileInfo& profile) {
        if (profile) {
            logger.info("Loading {} profile", profile.name);
            current = profile;

            if(updateVersion)
                updateVersion(profile.versionId);

            mods.reserve(profile.modNames.size());
            for (auto& modName : profile.modNames) {
                mods.emplace_back(modName);
            }

            for (auto& modinfo : mods) {
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

    const ProfileInfo& Manager::GetLaunchedProfile() {
        static ProfileInfo info;

        if (!info) {
            json::Document prefDocument = JsonHelper::OpenFile(Folder + "\\preferences.json");
            if (!prefDocument.IsNull()) {
                std::string profileHash = JsonHelper::FindString(prefDocument, "selectedProfile");
                if (!profileHash.empty()) {
                    json::Document profilesDocument = JsonHelper::OpenFile(Folder + "\\profiles.json");
                    if (!profilesDocument.IsNull()) {
                        info = JsonHelper::FindMember(profilesDocument, profileHash);
                    }
                }
            }
        }

        return info;
    }

    void Manager::setVersionCallback(std::function<void(const std::string&)> callback) {
        updateVersion = callback;
    }
}