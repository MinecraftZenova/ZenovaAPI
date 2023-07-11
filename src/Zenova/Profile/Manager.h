#pragma once

#include <chrono>
#include <deque>
#include <functional>
#include <vector>

#include "ProfileInfo.h"
#include "Zenova/Mod.h"

namespace Zenova {
    struct ModContext;

    class Manager {
        std::vector<ProfileInfo> profiles;
        std::deque<ModContext> mods;

        ProfileInfo launched;
        ProfileInfo current;
	public:
        std::string dataFolder;

        Manager();
        ~Manager();
        void init();
        void run();
        void update();
        void load(const ProfileInfo& profile);
        void swap(const ProfileInfo& profile);
        std::string getVersion();
        std::deque<ModContext>& getMods();

    private:
        void refreshList();
        ProfileInfo getProfile(const std::string& name);

        ProfileInfo _loadLaunchedProfile();
	};

    inline Manager manager;
}
