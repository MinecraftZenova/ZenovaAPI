//Meant to be used with ZenovaAPI, not for general use

#pragma once

#include <unordered_map>

#include "Common.h"

namespace Zenova {
	class StorageResolver {
	public:
		static std::tstring minecraft_path;
		static std::unordered_map<std::wstring, std::wstring> mirror_directory;

		StorageResolver();
		StorageResolver(const std::tstring& directory, const std::tstring& mirror);
		StorageResolver(const std::vector<std::tstring>& directories, const std::vector<std::tstring>& mirrors);

		//false on already exists, true on success
		bool addMirrorDirectory(const std::tstring& directory, const std::tstring& mirror);
		bool addMirrorDirectory(const std::vector<std::tstring>& directories, const std::vector<std::tstring>& mirrors);
	};
}