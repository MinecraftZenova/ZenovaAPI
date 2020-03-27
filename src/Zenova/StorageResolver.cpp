#include "Zenova/StorageResolver.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#include <comdef.h>

#include <iostream>
#include <array>

#include "Zenova/Log.h"

namespace Zenova {
	std::tstring StorageResolver::minecraft_path;
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
			Info(minecraft_path);
		}
		else {
			_com_error err(code);
			Error(err.ErrorMessage());
		}
	}

	StorageResolver::StorageResolver(const std::tstring& directory, const std::tstring& mirror) : StorageResolver() {
		addMirrorDirectory(directory, mirror);
	}

	StorageResolver::StorageResolver(const std::vector<std::tstring>& directories, const std::vector<std::tstring>& mirrors) : StorageResolver() {
		addMirrorDirectory(directories, mirrors);
	}

	bool StorageResolver::addMirrorDirectory(const std::tstring& directory, const std::tstring& mirror) {


		return true;
	}

	bool StorageResolver::addMirrorDirectory(const std::vector<std::tstring>& directories, const std::vector<std::tstring>& mirrors) {
		
		return true;
	}
}