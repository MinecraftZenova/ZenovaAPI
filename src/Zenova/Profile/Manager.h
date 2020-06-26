#pragma once

#include <vector>

#include "ProfileInfo.h"
#include "ModInfo.h"
#include <Zenova/Common.h>

namespace Zenova {
    class Manager {
        std::vector<ProfileInfo> profiles;
        std::vector<ModInfo> mods;

        const ProfileInfo launched;
        ProfileInfo current;

        void RefreshList();
        ProfileInfo GetProfile(const std::string& name);

	public:
        Manager();
        void Update();
        void Load(const ProfileInfo& profile);
        void Swap(const ProfileInfo& profile);
        EXPORT std::string GetLaunchedVersion() const;
        const ProfileInfo& GetLaunched() const;
	};
}