#pragma once

#include <chrono>
#include <functional>
#include <vector>

#include <Zenova/Common.h>

#include "ProfileInfo.h"
#include "ModInfo.h"

namespace Zenova {
    class Manager {
        std::vector<ProfileInfo> profiles;
        std::vector<ModInfo> mods;

        ProfileInfo launched;
        ProfileInfo current;

        std::function<void(const std::string&)> updateVersion;

        using clock = std::chrono::steady_clock;
        std::chrono::time_point<clock> tickTimer = clock::now();

	public:
        Manager();
        void Init();
        void Update();
        void Load(const ProfileInfo& profile);
        void Swap(const ProfileInfo& profile);
        const ProfileInfo& GetLaunchedProfile();

        void setVersionCallback(std::function<void(const std::string&)> callback);

    private:
        void RefreshList();
        ProfileInfo GetProfile(const std::string& name);
	};
}