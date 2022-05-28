#define _CRT_SECURE_NO_WARNINGS // std::getenv

#include "Manager.h"

#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/Utils/Utils.h"

namespace Zenova {
    // this needs to run before initcpp{var_addrs}, it currently does
    Manager::Manager() {
        dataFolder = []() -> std::string {
            std::string folder[] = {
                std::getenv("ZENOVA_DATA"),
                ::Util::GetAppDirectoryA() + "\\data", // this seems to return the minecraft exe directory :/
            };

            for (auto& str : folder) {
                if (!str.empty() && Util::IsFile(str + "\\ZenovaAPI.dll")) {
                    return str;
                }
            }

            return "";
        }();

        launched = getLaunchedProfile();
    }

    void Manager::init() {
        refreshList();
    }
    
    //doesn't unload current profile
    void Manager::refreshList() {
        profiles.clear();

        json::Document profilesDocument = JsonHelper::OpenFile(dataFolder + "\\profiles.json");
        if(!profilesDocument.IsNull() && profilesDocument.IsArray()) {
            for(auto& profile : profilesDocument.GetArray()) {
                if(launched.versionId == JsonHelper::FindString(profile, "versionId")) {
                    profiles.push_back(profile);
                }
            }
        }
    }

    ProfileInfo Manager::getProfile(const std::string& name) {
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

    void Manager::update() {
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

    void Manager::load(const ProfileInfo& profile) {
        if (profile) {
            logger.info("Loading {} profile", profile.name);
            current = profile;

            storage.setPath(profile.storagePath);

            mods.reserve(profile.modNames.size());
            for (auto& modName : profile.modNames) {
                ModInfo mod(modName);

                if (mod.mMod) {
                    mods.push_back(std::move(mod));
                }
            }

            for (auto& modinfo : mods) {
                modinfo.mMod->Start();
            }
        }
        else {
            logger.error("Empty Profile");
        }
    }

    void Manager::swap(const ProfileInfo& profile) {
        mods.clear();

        load(profile);
    }

    const ProfileInfo& Manager::getLaunchedProfile() {
        static ProfileInfo info;

        if (!info) {
            json::Document prefDocument = JsonHelper::OpenFile(dataFolder + "\\preferences.json");
            if (!prefDocument.IsNull()) {
                std::string profileHash = JsonHelper::FindString(prefDocument, "selectedProfile");
                if (!profileHash.empty()) {
                    json::Document profilesDocument = JsonHelper::OpenFile(dataFolder + "\\profiles.json");
                    if (!profilesDocument.IsNull()) {
                        info = JsonHelper::FindMember(profilesDocument, profileHash);
                    }
                }
            }
        }

        return info;
    }

    std::string Manager::getVersion() {
        return launched.versionId;
    }
}