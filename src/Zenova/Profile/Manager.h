#pragma once

#include <chrono>
#include <functional>
#include <vector>

#include "ProfileInfo.h"
#include "ModInfo.h"

namespace Zenova {
    class Manager {
        std::vector<ProfileInfo> profiles;
        std::vector<ModInfo> mods;

        ProfileInfo launched;
        ProfileInfo current;

        using clock = std::chrono::steady_clock;
        std::chrono::time_point<clock> tickTimer = clock::now();

	public:
        std::string dataFolder;

        Manager();
        ~Manager();
        void init();
        void run();
        void update();
        void load(const ProfileInfo& profile);
        void swap(const ProfileInfo& profile);
        void loadMod(const std::string& modName);
        std::string getVersion();
        size_t getModCount();

    private:
        void refreshList();
        ProfileInfo getProfile(const std::string& name);

        ProfileInfo _loadLaunchedProfile();
	};
}