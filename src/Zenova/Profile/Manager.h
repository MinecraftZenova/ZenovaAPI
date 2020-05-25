#pragma once

#include <vector>

#include "ProfileInfo.h"
#include "ModInfo.h"

namespace Zenova {
    class Manager {
        std::vector<ProfileInfo> profiles;
        std::vector<ModInfo> mods;

        const ProfileInfo launched;
        ProfileInfo current;

        void RefreshList();
        ProfileInfo GetProfile(const std::string& name);
        void Load(const ProfileInfo& profile);
        void Swap(const ProfileInfo& profile);

	public:
        Manager();
        void Update();
	};
}