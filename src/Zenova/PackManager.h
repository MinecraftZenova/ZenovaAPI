#pragma once

#include <vector>
#include <string>

namespace Zenova {
    class PackManager {
		PackManager();

	public:
		std::vector<std::pair<std::string, std::string>> resource_packs;
		std::vector<std::pair<std::string, std::string>> behavior_packs;

		bool AddMod(const std::string& path);

		static PackManager& instance();
	};
}