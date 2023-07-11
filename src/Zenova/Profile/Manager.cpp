#define LOG_CONTEXT "Zenova::Manager"

#include "Manager.h"

#include "Zenova/Log.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/Utils/Utils.h"

#include "ModInfo.h"

MOD_FUNCTION void PluginAddMod(Zenova::ModContext& mod);

namespace Zenova {
    // this currently runs (and has to) before initcpp{var_addrs}
    // todo: make launched const and ensure dataFolder is valid before getLaunchedProfile() 
    Manager::Manager() {
        dataFolder = Platform::GetZenovaFolder();
        launched = _loadLaunchedProfile();
    }

    Manager::~Manager() {
        // todo: verify this runs before ModInfo::~ModInfo
        for (auto& mod : mods) {
            CALL_MOD_FUNC(mod.info->mHandle, ModStop);
        }
    }

    void Manager::init() {
        refreshList();
        load(launched);
    }

    // doesn't unload current profile
    void Manager::refreshList() {
        profiles.clear();

        json::Document profilesDocument = JsonHelper::OpenFile(dataFolder + "\\profiles.json");
        if (!profilesDocument.IsNull() && profilesDocument.IsArray()) {
            for (auto& profile : profilesDocument.GetArray()) {
                if (launched.versionId == JsonHelper::FindString(profile, "versionId")) {
                    profiles.push_back(profile);
                }
            }
        }
    }

    ProfileInfo Manager::getProfile(const std::string& name) {
        auto profileIter = std::find_if(profiles.begin(), profiles.end(),
            [&name](const ProfileInfo& p) { return p.name == name; });
        if (profileIter != profiles.end()) {
            return *profileIter;
        }

        Zenova_Error("{} does not exist in profile list with version: {}", name, launched.versionId);
        return ProfileInfo();
    }

    void Manager::run() {
        for (auto& mod : mods) {
            CALL_MOD_FUNC(mod.info->mHandle, ModStart);
        }
    }

    void Manager::update() {
        for (auto& mod : mods) {
            CALL_MOD_FUNC(mod.info->mHandle, ModUpdate);
        }
    }

    void Manager::load(const ProfileInfo& profile) {
        if (!profile) {
            // todo: should we dump the full ProfileInfo?
            Zenova_Error("Failed to load profile {}, versionId is empty", profile.name);
            return;
        }

        Zenova_Info("Loading {} profile", profile.name);
        current = profile;

        std::string versionFolder = dataFolder + "\\versions\\Minecraft-" + profile.versionId + "\\";
        ModInfo* api = new ModInfo(versionFolder, "BedrockAPI");
        ModContext apiContext{ api };
        if (api->mHandle) {
            CALL_MOD_FUNC(api->mHandle, ModLoad, apiContext);
        }
        else {
            Zenova_Error("Failed to load {}, skipping all mods", api->mNameId);
            Platform::ErrorPrinter();
            return;
        }

        for (auto& modName : profile.modNames) {
            Zenova_Info("Loading {}", modName);

            std::string folder = dataFolder + "\\mods\\" + modName + "\\";
            ModInfo* mod = new ModInfo(folder);

            if (mod->mHandle) {
                mods.push_back(ModContext{ mod });
            }
            else {
                Zenova_Warn("Failed to load {}", mod->mNameId);
                Platform::ErrorPrinter();
            }
        }

        for (auto& mod : mods) {
            CALL_MOD_FUNC(api->mHandle, PluginAddMod, mod);
            CALL_MOD_FUNC(mod.info->mHandle, ModLoad, mod);
        }

        mods.push_front(std::move(apiContext));
    }

    void Manager::swap(const ProfileInfo& profile) {
        mods.clear();

        load(profile);
    }

    std::string Manager::getVersion() {
        return launched.versionId;
    }

    std::deque<ModContext>& Manager::getMods() {
        return mods;
    }

    // todo: should we stop execution for these errors?
    ProfileInfo Manager::_loadLaunchedProfile() {
        json::Document prefDocument = JsonHelper::OpenFile(dataFolder + "\\preferences.json");
        if (prefDocument.IsNull()) {
            Zenova_Error("preferences.json is empty");
            return {};
        }

        std::string profileHash = JsonHelper::FindString(prefDocument, "selectedProfile");
        if (profileHash.empty()) {
            Zenova_Error("profileHash is empty");
            return {};
        }

        json::Document profilesDocument = JsonHelper::OpenFile(dataFolder + "\\profiles.json");
        if (profilesDocument.IsNull()) {
            Zenova_Error("profiles.json is empty");
            return {};
        }

        return JsonHelper::FindMember(profilesDocument, profileHash);
    }
}