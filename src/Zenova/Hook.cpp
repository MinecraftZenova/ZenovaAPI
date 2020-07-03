#include "Zenova/Hook.h"

#include "Zenova/Globals.h"

#include <list>

namespace Zenova {
	namespace Hook {
		std::unordered_map<std::string, uintptr_t> functions;
		std::unordered_map<std::string, uintptr_t> vtables;
		std::unordered_map<std::string, uintptr_t> variables;
		//std::unordered_map<std::string, std::unordered_map<std::string, uintptr_t>> Symbols;

		uintptr_t SlideAddress(size_t offset) {
			return BaseAddress + offset;
		}

		uintptr_t UnslideAddress(uintptr_t result) {
			return result - BaseAddress;
		}

		/*
		uintptr_t FindSymbol(const char* scope, const char* funcName) {
			auto scopeIter = Symbols.find(scope);

			if(scopeIter != Symbols.end()) {
				auto funcIter = scopeIter->second.find(funcName);

				if((funcIter == scopeIter->second.end()) && funcIter->second) {
					Zenova_Info(std::string(funcName) + " function not found");
				}
				else {
					return funcIter->second;
				}
			}
			else {
				Zenova_Info(std::string(scope) + " scope not found");
			}
			return 0;
		}

		inline std::string ParameterCleanup(const std::string& function) {
			std::size_t parameterStart = function.find("(");
			if(parameterStart == std::string::npos) { //not a function
				return function;
			}

			//std::size_t parameterEnd = function.rfind(")");
			std::string func = function;

			std::vector<std::string> toErase = {
				"struct ",
				"class ",
				"enum ",
				"__cdecl ",
				"__clrcall ",
				"__fastcall ",
				"__thiscall ",
				"__vectorcall "
			};

			for(auto& str : toErase) {
				std::size_t stlPos = 0;
				while((stlPos = func.find(str, stlPos)) != std::string::npos) {
					func.erase(stlPos, str.length());
				}
			}

			//change this later to support things like "std::vector<std::string>>"
			std::vector<std::pair<std::string, std::string>> stlStrings = {
				std::make_pair("std::basic_string<char", "std::string")
			};

			//for(auto& [first, second] : stlStrings) {
			//	std::size_t stlPos = 0;
			//	while((stlPos = func.find(first, stlPos)) != std::string::npos) {
			//		func.replace(stlPos, (func.find(" >", stlPos) + 2) - stlPos, second);
			//	}
			//}

			std::vector<std::string> types = {
				"&",
				"*",
				","
			};

			for(auto& type : types) {
				std::size_t refPos = 0;
				while((refPos = func.find(type, refPos)) != std::string::npos) {
					if(func.at(refPos - 1) == ' ') {
						func.erase(--refPos, 1);
					}

					char afterChar = func.at(++refPos);
					if((afterChar != ' ') && (afterChar != ',') && (afterChar != ')')) {
						func.insert(refPos, " ");
					}
				}
			}

			return func;
		}

		uintptr_t FindSymbol(const char* function) {
			std::string namespaceStr = "global", functionName = function;
			std::size_t namespaceEnd = functionName.find("::", functionName.rfind(" ", functionName.find("(")));
			if(namespaceEnd != std::string::npos) {
				std::size_t namespaceStart = functionName.rfind(" ", namespaceEnd) + 1;
				namespaceStr = functionName.substr(namespaceStart, namespaceEnd - namespaceStart);
				functionName = ParameterCleanup(functionName.substr(0, namespaceStart) + functionName.substr(namespaceEnd + 2));
			}
			return FindSymbol(namespaceStr.c_str(), functionName.c_str());
		}
		*/

		uintptr_t FindAddressHelper(const std::unordered_map<std::string, uintptr_t>& symbolMap, const std::string& symbol, const std::string& mapName) {
			auto symbolMapIter = symbolMap.find(symbol);

			if(symbolMapIter != symbolMap.end()) {
				return symbolMapIter->second;
			}

			logger.warn("{} in {} not found", symbol, mapName);
			return 0;
		}

		uintptr_t FindMangledSymbol(const char* function) {
			return FindAddressHelper(functions, function, "function");
		}

		uintptr_t FindVTable(const char* vtable) {
			return FindAddressHelper(vtables, vtable, "vtable");
		}

		uintptr_t FindVariable(const char* variable) {
			return FindAddressHelper(variables, variable, "variable");
		}

		bool MemCompare(const char* data, const char* sig, const char* mask) {
			for (; *mask; ++mask, ++data, ++sig) {
				if (*mask == 'x' && *data != *sig) {
					return false;
				}
			}

			return true;
		}

		uintptr_t Sigscan(const char* sig, const char* mask) {
			auto size = Platform::GetModuleSize("Minecraft.Windows.exe");

			for (size_t i = 0; i < size; ++i) {
				if (MemCompare(reinterpret_cast<const char*>(BaseAddress + i), sig, mask)) {
					return BaseAddress + i;
				}
			}

			return NULL;
		}

		uintptr_t SigscanCall(const char* sig, const char* mask) {
			auto size = Platform::GetModuleSize("Minecraft.Windows.exe");

			for (size_t i = 0; i < size; ++i) {
				if (MemCompare(reinterpret_cast<const char*>(BaseAddress + i), sig, mask)) {
					uintptr_t offset = BaseAddress + i + 1;
					return offset + 4 + *reinterpret_cast<int*>(offset);
				}
			}

			return NULL;
		}

		class MemoryState {
			u8* mLocation;
			std::size_t mSize;
			std::vector<u8> mBytes;
			u32 mProtection;

		public:
			MemoryState(u8* location, std::size_t size) : mLocation(location), mSize(size) {
				mProtection = Zenova::Platform::SetPageProtect(location, size, 0x40);

				mBytes.reserve(size);

				for(auto i = 0; i < size; ++i) {
					mBytes.push_back(location[i]);
				}
			}

			~MemoryState() {
				for(auto i = 0; i < mSize; ++i) {
					mLocation[i] = mBytes.at(i);
				}

				Zenova::Platform::SetPageProtect(mLocation, mSize, mProtection);
			}
		};

		//this is platform and arch dependent right now, need to add support for others in the future 
		bool Create(void* function, void* funcJump, void* funcTrampoline) {
			bool successful = false;

			if(Platform::Type == Platform::PlatformType::Windows) {
				u8* u8Function = reinterpret_cast<u8*>(function);

				if(*(u8Function + 7) == 0xff) {
					u8* retLocation = u8Function + 7;
					MemoryState state(retLocation, 2);

					*(retLocation) = 0xc3;
					*(retLocation + 1) = 0xcc;

					uintptr_t address = reinterpret_cast<uintptr_t(*)()>(function)();
					successful = Zenova::Platform::CreateHook(reinterpret_cast<void*>(address), funcJump, reinterpret_cast<void**>(funcTrampoline));
				}
			}

			return successful;
		}

		bool Create(void* vtable, void* function, void* funcJump, void* funcTrampoline) {
			bool successful = false;

			if(Platform::Type == Platform::PlatformType::Windows) {
				u8* u8Function = reinterpret_cast<u8*>(function);

				if(*(u8Function + 3) == 0xff) {
					uintptr_t address = reinterpret_cast<uintptr_t>(vtable) + *reinterpret_cast<u32*>(u8Function + 5);
					successful = Zenova::Platform::CreateHook(reinterpret_cast<void*>(address), funcJump, reinterpret_cast<void**>(funcTrampoline));
				}
			}

			return successful;
		}
	}
}