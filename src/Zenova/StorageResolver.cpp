#include "Zenova/StorageResolver.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#include <comdef.h>

#include <iostream>
#include <array>

#include "Globals.h"
#include "Zenova/Log.h"

namespace Zenova {
	UniversalString StorageResolver::minecraft_path;
	std::unordered_map<std::wstring, std::wstring> StorageResolver::mirror_directory;

	StorageResolver::StorageResolver() {
		//will have to change this later to meet 1607 expectations of really long filepaths
		std::array<wchar_t, 1000> szPathW;
		HRESULT code = SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPathW.data());
		if(SUCCEEDED(code)) {
			// Get the path to the textures folder
			std::wstring_view szPathView(szPathW.data());
			std::wstring appData(szPathView.substr(0, szPathView.rfind(L"AC")));
			appData += L"LocalState/games/com.mojang/";

			//Change all the trailing backslashes into forward slash
			std::wstring_view replaced = L"\\";
			for(size_t start = 0, pos = 0, end = appData.find(L"LocalState"); start != end; start = pos + replaced.length()) {
				pos = appData.find(replaced, start);
				appData.replace(pos, replaced.length(), L"/");
			}

			//SHGetFolderPathW doesn't properly captialize this, use lowercase in the future?
			replaced = L"microsoft.minecraftuwp";
			appData.replace(appData.find(replaced), replaced.length(), L"Microsoft.MinecraftUWP");

			minecraft_path = appData;
			logger.info(minecraft_path);
		}
		else {
			_com_error err(code);
			logger.error(err.ErrorMessage());
		}
	}

	StorageResolver::StorageResolver(const UniversalString& directory, const UniversalString& mirror) : StorageResolver() {
		addMirrorDirectory(directory, mirror);
	}

	StorageResolver::StorageResolver(const std::vector<UniversalString>& directories, const std::vector<UniversalString>& mirrors) : StorageResolver() {
		addMirrorDirectory(directories, mirrors);
	}

	bool StorageResolver::addMirrorDirectory(const UniversalString& directory, const UniversalString& mirror) {


		return true;
	}

	bool StorageResolver::addMirrorDirectory(const std::vector<UniversalString>& directories, const std::vector<UniversalString>& mirrors) {
		
		return true;
	}
}