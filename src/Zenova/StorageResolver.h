//Meant to be used with ZenovaAPI, not for general use

#pragma once

#include <unordered_map>

#include "Zenova/Common.h"

namespace Zenova {
	class StorageResolver {
	public:
		static UniversalString minecraft_path;
		static std::unordered_map<std::wstring, std::wstring> mirror_directory;

		StorageResolver();
		StorageResolver(const UniversalString& directory, const UniversalString& mirror);
		StorageResolver(const std::vector<UniversalString>& directories, const std::vector<UniversalString>& mirrors);

		//false on already exists, true on success
		bool addMirrorDirectory(const UniversalString& directory, const UniversalString& mirror);
		bool addMirrorDirectory(const std::vector<UniversalString>& directories, const std::vector<UniversalString>& mirrors);
	};
}