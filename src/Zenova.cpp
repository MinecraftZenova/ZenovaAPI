#include "Zenova.h"

#include <iostream> //std::cout (Zenova::MessageRedirection)
#include <algorithm> //std::find_if
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cstdlib>

#include "Zenova/StorageResolver.h"
#include "Utils/Utils.h"

#ifdef _MSC_VER
#undef GetObject
#endif

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

namespace json = rapidjson;

namespace Zenova {
	class MessageRedirection { //currently used to make a console for the app
		FILE* console = nullptr;

	public:
		MessageRedirection() {
			if(!AllocConsole()) {
				Platform::ErrorPrinter();
				return;
			}

			if(!console) {
				//Handle std::cout, std::clog, std::cerr, std::cin
				freopen_s(&console, "CONOUT$", "w", stdout);
				freopen_s(&console, "CONOUT$", "w", stderr);
				freopen_s(&console, "CONIN$", "r", stdin);

				HANDLE hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				HANDLE hConIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
				SetStdHandle(STD_ERROR_HANDLE, hConOut);
				SetStdHandle(STD_INPUT_HANDLE, hConIn);

				std::cout.clear();
				std::clog.clear();
				std::cerr.clear();
				std::cin.clear();
				std::wcout.clear();
				std::wclog.clear();
				std::wcerr.clear();
				std::wcin.clear();
			}
		}

		~MessageRedirection() {
			if(!FreeConsole()) {
				Platform::ErrorPrinter();
			}

			if(console) {
				fclose(console);
			}
		}
	};
	
#ifdef ZENOVA_API
	inline std::string ZenovaFolder = "";

	void LoadSymbolMaps() {
		/* Currently a horrid implementation

		std::ifstream symFile(ZenovaFolder + "/SymbolMap.json");
		json::IStreamWrapper symStream(symFile);
		json::Document symbols;
		symbols.ParseStream(symStream);

		if(symbols.IsObject()) {
			for(auto itr = symbols.MemberBegin(); (itr != symbols.MemberEnd()) && itr->value.IsArray(); ++itr) {
				for (json::SizeType i = 0; i < itr->value.Size(); i++) {
					auto& obj = itr->value[i];
					if(obj.IsObject() && obj.HasMember("name")) {
						std::string funcName = obj["name"].GetString();
						uintptr_t address = Hook::BaseAddress;

						if(obj.HasMember("signature")) {
							//find address with IDA's signature
						}

						if((address == Hook::BaseAddress) && obj.HasMember("address")) {
							auto& addr = obj["address"];
							if(addr.IsNumber()) {
								address += addr.GetUint64();
							}
							else if(addr.IsString()) {
								address += std::strtoull(addr.GetString(), nullptr, 16);
							}
						}

						if((address > Hook::BaseAddress) && obj.HasMember("functions")) {
							auto& vtableNames = obj["functions"];
							if(vtableNames.IsArray()) {
								for(json::SizeType i = 0; i < vtableNames.Size(); i++) {
									std::string objName = itr->name.GetString();
									Zenova_Info(std::string("Loaded ") + objName + ": " + funcName + "[" + ::Util::HexString(reinterpret_cast<uintptr_t*>(address)[i]) + "]");
									Hook::Symbols[funcName][vtableNames[i].GetString()] = reinterpret_cast<uintptr_t*>(address)[i];
								}
							}
						}

						if(address > Hook::BaseAddress) {
							std::string objName = itr->name.GetString();
							Zenova_Info(std::string("Loaded ") + objName + ": " + funcName + "[" + ::Util::HexString(address) + "]");
							Hook::Symbols[objName][funcName] = address;
						}
					}
				}
			}
		}*/
	}

	struct Version {
		u8 major = 0, minor = 0, build = 0, commit = 0;
	};

	namespace JsonHelper {
		json::Document OpenFile(const std::string& fileLocation) {
			json::Document d;
			std::ifstream ifs(fileLocation);
			if(ifs.is_open()) {
				d.ParseStream(json::IStreamWrapper(ifs));
			}
			else {
				Zenova_Warn("\"" + fileLocation + "\" not found"); 
				d.Parse("{}");
			}

			return d;
		}

		const json::Value& FindMember(const json::Value& obj, const std::string& memberStr) {
			auto& objIter = obj.FindMember(memberStr.c_str());
			if(objIter != obj.MemberEnd()) {
				return objIter->value;
			}
			
			Zenova_Warn("Couldn't find " + memberStr);
			static const json::Value null;
			return null;
		}

		std::string FindString(const json::Value& rootObject, const std::string& memberStr) {
			auto& objStr = JsonHelper::FindMember(rootObject, memberStr);
			if(objStr.IsString()) {
				return objStr.GetString();
			}
			else {
				Zenova_Warn(memberStr + " isn't a String");
			}

			return "";
		}

		std::string FindVersion(const json::Value& obj, const std::string& memberStr) {
			std::string retString;
			auto& version = JsonHelper::FindMember(obj, memberStr);
			if(version.IsArray()) {
				for(auto& versionNum : version.GetArray()) {
					if(versionNum.IsUint64()) retString += std::to_string(versionNum.GetUint64()) + '.';
				}

				if(!retString.empty()) retString.pop_back();
			}
			else if(version.IsString()) {
				//versionId can be latest-release or latest-beta, we're assuming those won't be launched with zenova
				return version.GetString();
			}

			return retString;
		}
	}

	class Profile {
	public:
		std::string name, created, lastUsed, versionId;
		std::vector<std::string> modNames;

		Profile() {}
		Profile(const json::Value& profile) {
			if(profile.IsObject()) {
				name = JsonHelper::FindString(profile, "name");
				created = JsonHelper::FindString(profile, "created");
				lastUsed = JsonHelper::FindString(profile, "lastUsed");
				versionId = JsonHelper::FindVersion(profile, "versionId");
			
				auto& mods = JsonHelper::FindMember(profile, "mods");
				if(mods.IsArray()) {
					for(auto& mod : mods.GetArray()) {
						if(mod.IsString()) {
							modNames.push_back(mod.GetString());
						}
					}
				}
				else if(mods.IsString()) {
					modNames.push_back(mods.GetString());
				}
				else {
					Zenova_Warn("No mods found in loaded profile");
				}
			}
		}
	};

	struct ModInfo {
		Mod* mod = nullptr;
		void* handle = nullptr;
		std::string nameId, name = "", description = "";
		std::string version, minVersion, maxVersion;

		ModInfo(const json::Value& modinfo) {
			if(modinfo.IsObject()) {
				nameId = JsonHelper::FindString(modinfo, "nameId");
				name = JsonHelper::FindString(modinfo, "name");
				description = JsonHelper::FindString(modinfo, "description");
				version = JsonHelper::FindVersion(modinfo, "version");
				minVersion = JsonHelper::FindVersion(modinfo, "minVersion");
				maxVersion = JsonHelper::FindVersion(modinfo, "maxVersion");
			}
		}
	};

	class Manager {
		const Profile launched;
		Profile current;
		std::vector<Profile> profiles;
		std::vector<ModInfo> mods;
	
		Profile GetLaunchedProfile() {
			json::Document prefDocument = JsonHelper::OpenFile(ZenovaFolder + "/preferences.json");
			if(prefDocument.IsObject()) {
				std::string profileHash = JsonHelper::FindString(prefDocument.GetObject(), "selectedProfile");
				if(!profileHash.empty()) {
					json::Document profilesDocument = JsonHelper::OpenFile(ZenovaFolder + "/profiles.json");
					if(profilesDocument.IsObject()) {
						auto& member = JsonHelper::FindMember(profilesDocument.GetObject(), profileHash);
						if(!member.IsNull()) 
							return member;
					}
				}
			}

			return Profile();
		}

	public:
		Manager() : launched(GetLaunchedProfile()) {
			RefreshProfileList();
			LoadProfile(launched);
			LoadSymbolMaps();
		}

		~Manager() {
			Destroy();
		}

		//doesn't unload current profile
		void RefreshProfileList() {
			profiles.clear();

			json::Document profilesDocument = JsonHelper::OpenFile(ZenovaFolder + "/profiles.json");
			if(profilesDocument.IsArray()) {
				for(auto& profile : profilesDocument.GetArray()) {
					if(launched.versionId == JsonHelper::FindString(profile, "versionId")) {
						profiles.push_back(profile);
					}
				}
			}
		}

		void LoadMod(const std::string& mod) {
			Zenova_Info("Loading " + mod);

			std::string folder = ZenovaFolder + "\\mods\\" + mod + "\\";
			std::string fileLocation = folder + "modinfo.json";
			std::ifstream profilesStream(fileLocation);
			if(profilesStream.is_open()) {
				json::Document modDocument;
				modDocument.ParseStream(json::IStreamWrapper(profilesStream));

				if(modDocument.IsObject()) {
					ModInfo modInfo(modDocument.GetObject());

					modInfo.handle = Platform::LoadModule(folder + modInfo.nameId);
					if(modInfo.handle) {
						using FuncPtr = Mod * (*)();
						FuncPtr createMod = reinterpret_cast<FuncPtr>(Platform::GetModuleFunction(modInfo.handle, "CreateMod"));

						if(createMod) {
							modInfo.mod = createMod();
							mods.push_back(modInfo);
						}
						else {
							Zenova_Warn("Failed to find CreateMod in " + modInfo.nameId);
						}
					}
					else {
						Zenova_Warn("Failed to load " + modInfo.name);
						Platform::ErrorPrinter();
					}
				}
			}
			else {
				Zenova_Warn("\"" + fileLocation + "\" not found");
			}
		}

		void LoadProfile(const Profile& profile) {
			Zenova_Info("Loading " + profile.name + " profile");
			current = profile;

			for(auto& mod : profile.modNames) {
				LoadMod(mod);
			}

			for(auto& modinfo : mods) {
				modinfo.mod->Start();
			}
		}

		void LoadProfile(const std::string& profile) {
			auto profileIter = std::find_if(profiles.begin(), profiles.end(), 
				[&profile](const Profile& p) { return p.name == profile; });
			if(profileIter != profiles.end()) {
				LoadProfile(*profileIter);
			}
			else {
				Zenova_Error(profile + " does not exist in profile list with version: " + launched.versionId);
			}
		}

		void Swap(const std::string& profile) {
			Zenova_Info("Swapped to " + profile + " profile");
			Destroy();
			LoadProfile(profile);
			//todo edit launch time
		}

		void Destroy() {
			for(auto& modinfo : mods) {
				modinfo.mod->Stop();
			}

			for(auto& modinfo : mods) {
				Platform::CloseModule(modinfo.handle);
			}
			
			mods.clear();
		}

		void Update() {
			for(auto& modinfo : mods) {
				modinfo.mod->Update();
			}
		}

		void Tick() {
			for(auto& modinfo : mods) {
				modinfo.mod->Tick();
			}
		}
	};

	std::string GetZenovaFolder() {
		std::vector<std::string> folder = {
			std::getenv("ZENOVA_DATA"),
			::Util::GetAppDirectoryA() + "/Data",
		};

		for(auto& str : folder) {
			if(!str.empty() && Util::IsDirectory(str + "/versions")) {
				return str;
			}
		}

		return "";
	}

	unsigned long __stdcall Start(void* dllHandle) {
		ZenovaFolder = GetZenovaFolder();
		Hook::BaseAddress = Platform::GetModuleBaseAddress("Minecraft.Windows.exe");
		bool canRun = (Platform::Init(dllHandle) && !ZenovaFolder.empty() && Hook::BaseAddress);
		if(canRun) {
			MessageRedirection console;

			Zenova_Info("Zenova Started");
			Zenova_Info("ZenovaData Location: " + ZenovaFolder);
			Zenova_Info("Minecraft's BaseAddress: " + ::Util::HexString(Hook::BaseAddress));

			StorageResolver storage(L"minecraftWorlds/", L"D:/MinecraftBedrock/Worlds");
			Manager zenova;

			while(canRun) {
				//Works in the console window :P
				//if(GetAsyncKeyState(VK_ESCAPE)) { 
				//	canRun = false;
				//}

				zenova.Update();

				//Either add our own system using chrono or hook into Minecraft's "global" tick function
				zenova.Tick();
			}

			Zenova_Info("Zenova Stopped");
		}

		Platform::Destroy();
		return 0;
	}
#endif // ZENOVA_API
}