#include "Manager.h"
#include <filesystem>
#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Globals.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/PackManager.h"
#include "Zenova/Utils/Utils.h"

#define CALL_MOD_FUNC(mod, func, ...) \
    if (mod.mHandle) {  \
        auto modFunc = reinterpret_cast<decltype(&func)>(Platform::GetModuleFunction(mod.mHandle, #func)); \
        if (modFunc) { \
            modFunc(__VA_ARGS__); \
        } \
    }

namespace Zenova {
    // this currently runs (and has to) before initcpp{var_addrs}
    // todo: make launched const and ensure dataFolder is valid before getLaunchedProfile() 
    Manager::Manager() {
        dataFolder = Platform::GetZenovaFolder();
        launched = _loadLaunchedProfile();
    }

    Manager::~Manager() {
        // todo: verify this runs before ModInfo::~ModInfo
        for (auto& modinfo : mods) {
            CALL_MOD_FUNC(modinfo, ModStop);
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

        logger.error("{} does not exist in profile list with version: {}", name, launched.versionId);
        return ProfileInfo();
    }

    void Manager::run() {
        for (auto& modinfo : mods) {
            CALL_MOD_FUNC(modinfo, ModStart);
        }
    }

    void Manager::update() {
        // todo: hook into minecraft's global tick function
        namespace chrono = std::chrono;
        using tick = chrono::duration<int, std::ratio<1, 20>>;

        if (chrono::duration_cast<tick>(clock::now() - tickTimer).count() >= 1) {
            tickTimer = clock::now();

            for (auto& modinfo : mods) {
                CALL_MOD_FUNC(modinfo, ModTick)
            }
        }
    }

    void Manager::load(const ProfileInfo& profile) {
        if (!profile) {
            // todo: should we dump the full ProfileInfo?
            logger.error("Failed to load profile {}, versionId is empty", profile.name);
            return;
        }

        logger.info("Loading {} profile", profile.name);
        current = profile;

        mods.reserve(profile.modNames.size());
        for (auto& modName : profile.modNames) {
            loadMod(modName);
        }
    }

    void Manager::swap(const ProfileInfo& profile) {
        mods.clear();

        load(profile);
    }

    void* Manager::loadMod(const std::string& modName)
    {
        if (std::find_if(mods.begin(), mods.end(),
        [&modName](const ModInfo& mod) { return mod.mNameId == modName; }) != mods.end())
            return nullptr;
            
        std::string folder = manager.dataFolder + "\\mods\\" + modName + "\\";
        if (!std::filesystem::exists(folder))
            return nullptr;

        logger.info("Loading {}", modName);

        // todo: verify path
        ModInfo mod(folder);
        void* modHandle = mod.loadModule();

        if (modHandle) {
            ModContext ctx = { folder };
            CALL_MOD_FUNC(mod, ModLoad, ctx)

            PackManager::addMod(folder);

            mods.push_back(std::move(mod));
            return modHandle;
        }
        else {
            logger.warn("Failed to load {}", mod.mName);
            Platform::ErrorPrinter();
            return nullptr;
        }
    }

    std::vector<ModInfo>& Manager::getMods()
    {
        return mods;
    }

    std::string Manager::getVersion() {
        return launched.versionId;
    }

    size_t Manager::getModCount() {
        return current.modNames.size();
    }

    // todo: should we stop execution for these errors?
    ProfileInfo Manager::_loadLaunchedProfile() {
        json::Document prefDocument = JsonHelper::OpenFile(dataFolder + "\\preferences.json");
        if (prefDocument.IsNull()) {
            logger.error("preferences.json is empty");
            return {};
        }

        std::string profileHash = JsonHelper::FindString(prefDocument, "selectedProfile");
        if (profileHash.empty()) {
            logger.error("profileHash is empty");
            return {};
        }

        json::Document profilesDocument = JsonHelper::OpenFile(dataFolder + "\\profiles.json");
        if (profilesDocument.IsNull()) {
            logger.error("profiles.json is empty");
            return {};
        }

        return JsonHelper::FindMember(profilesDocument, profileHash);
    }
}