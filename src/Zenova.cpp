#include "Zenova.h"

#include <iostream> //std::cout (Zenova::MessageRedirection)
#include <algorithm> //std::find_if
#include <fstream>
#include <cstdio>

#include "Zenova/StorageResolver.h"

namespace Zenova {
	//setup these macros with cmake
	#ifdef _WINDOWS
		std::shared_ptr<OS> hostOS = std::make_shared<Windows>();
		void ErrorPrinter() {
			// Convert GetLastError() to an actual string using the windows api
			LPVOID message_buffer{};
			const auto last_error = GetLastError();

			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  reinterpret_cast<LPSTR>(&message_buffer), 0, NULL);
			if (message_buffer == nullptr) {
				return;
			}

			Error(reinterpret_cast<LPSTR>(message_buffer));
			LocalFree(message_buffer);
		}
	#elif Linux
		std::shared_ptr<OS> hostOS = std::make_shared<Linux>();
	#endif

	class MessageRedirection { //currently used to make a console for the app
		FILE* console = nullptr;

	public:
		MessageRedirection() {
			if(!AllocConsole()) {
				ErrorPrinter();
				return;
			}

			if(!console) {
				//Handle std::cout, std::clog, std::cerr, std::cin
				freopen_s(&console, "CONOUT$", "w", stdout);
				freopen_s(&console, "CONOUT$", "w", stderr);
				freopen_s(&console, "CONIN$", "r", stdin);
				std::cout.clear();
				std::clog.clear();
				std::cerr.clear();
				std::cin.clear();

				HANDLE hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				HANDLE hConIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
				SetStdHandle(STD_ERROR_HANDLE, hConOut);
				SetStdHandle(STD_INPUT_HANDLE, hConIn);
				std::wcout.clear();
				std::wclog.clear();
				std::wcerr.clear();
				std::wcin.clear();
			}
		}
		~MessageRedirection() {
			if(!FreeConsole()) {
				ErrorPrinter();
			}

			if(console) {
				fclose(console);
			}
		}
	};
	
#ifdef ZENOVA_API
	std::string ZenovaFolder = "D:/Zenova";

	void LoadSymbolMaps() {

	}

	std::vector<std::pair<Mod*, HMODULE>> LoadMods(const std::vector<std::string>& mods) {
		std::vector<std::pair<Mod*, HMODULE>> ret;

		for(auto& mod : mods) {
			HMODULE hModule = LoadLibraryA(mod.c_str());
			if(hModule) {
				FARPROC createMod = GetProcAddress(hModule, "CreateMod");

				if(createMod) {
					ret.push_back(std::make_pair(reinterpret_cast<Mod*>(createMod()), hModule));
					continue;
				}
				else {
					Warn("Failed to find CreateMod in " + mod);
				}
			}
			else {
				ErrorPrinter();
				Warn(mod.substr(mod.rfind("/") + 1, mod.rfind(".dll")) + " failed to load");
			}
		}
		
		return ret;
	}
	
	struct Profile {
		std::vector<std::string> libraryNames;
	};

	Profile LoadProfile() {
		std::vector<std::string> libNames;
		
		WIN32_FIND_DATAA fileData;
		HANDLE dir;

		if((dir = FindFirstFileA((ZenovaFolder + "/Mods/*").c_str(), &fileData)) != INVALID_HANDLE_VALUE) {
			do {
				const std::string fileName = fileData.cFileName;
				const std::string filePath = ZenovaFolder + "/Mods/" + fileName;

				if((fileName[0] == '.') || ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
					continue;

				if(fileName.rfind(".dll") != fileName.npos) {
					libNames.push_back(filePath);
				}
			} while(FindNextFile(dir, &fileData));
		}

		return {libNames};
	}

	unsigned long __stdcall Start(void* dllHandle) {
		Windows::DebugPause();

		Hook::BaseAddress = Util::GetModuleBaseAddressA("Minecraft.Windows.exe");
		MessageRedirection console;

		Info("Zenova Started");
		Info("Minecraft at: " + Util::to_hex_string(Hook::BaseAddress));
	
		StorageResolver storage(L"minecraftWorlds/", L"D:/MinecraftBedrock/Worlds");
		
		if(hostOS->type == OSType::Windows) {
			hostOS->SetupOS(dllHandle);
		}
		
		Profile profile = LoadProfile();
		std::vector<std::pair<Mod*, HMODULE>> mods = LoadMods(profile.libraryNames);
		
		for(auto [first, second] : mods) {
			first->Start();
		}

		bool run = true;
		while(run) {
			for(auto [first, second] : mods) {
				first->Update();
			}
		}
		
		for(auto [first, second] : mods) {
			first->Stop();
			FreeLibrary(second);
		}

		Info("Zenova Stopped");
		return 0;
	}
#endif // ZENOVA_API
}