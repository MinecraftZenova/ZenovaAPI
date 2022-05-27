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
			return Platform::GetMinecraftBaseAddress() + offset;
		}

		uintptr_t UnslideAddress(uintptr_t result) {
			return result - Platform::GetMinecraftBaseAddress();
		}

		uintptr_t FindAddressHelper(const std::unordered_map<std::string, uintptr_t>& symbolMap, const char* symbol, const char* mapName) {
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

		//doesn't work for inlined dtors, need to put an error message when there's a vtable load at the start
		uintptr_t GetRealDtor(uintptr_t virtualDtor) {
			virtualDtor = *reinterpret_cast<uintptr_t*>(virtualDtor); 
			u8* iaddr = reinterpret_cast<u8*>(virtualDtor);
			if (MemCompare(reinterpret_cast<const char*>(iaddr), "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x8B\xDA\x48\x8B\xF9", "xxxx?xxxxxxxxxx")) {
				return virtualDtor + 0x14 + *reinterpret_cast<int32_t*>(iaddr + 0x10);
			}

			Zenova_Warn("0x{:x} is not a valid virtual dtor, dtor maybe inlined?", virtualDtor);
			return 0;
		}

		uintptr_t Sigscan(const char* sig, const char* mask) {
			auto size = Platform::GetMinecraftSize();

			for (size_t i = 0; i < size; ++i) {
				uintptr_t iaddr = SlideAddress(i);
				if (MemCompare(reinterpret_cast<const char*>(iaddr), sig, mask)) {
					return iaddr;
				}
			}

			return 0;
		}

		uintptr_t SigscanCall(const char* sig, const char* mask) {
			auto size = Platform::GetMinecraftSize();

			for (size_t i = 0; i < size; ++i) {
				uintptr_t iaddr = SlideAddress(i);
				if (MemCompare(reinterpret_cast<const char*>(iaddr), sig, mask)) {
					uintptr_t offset = iaddr + 1;
					return offset + 4 + *reinterpret_cast<int32_t*>(offset);
				}
			}

			return 0;
		}

		// todo: remove?
		class MemoryState {
			u8* mLocation;
			std::size_t mSize;
			std::vector<u8> mBytes;
			u32 mProtection;

		public:
			MemoryState(u8* location, std::size_t size) : mLocation(location), mSize(size) {
				mProtection = Platform::SetPageProtect(location, size, ProtectionFlags::Execute | ProtectionFlags::Write);

				mBytes.reserve(size);

				for(auto i = 0; i < size; ++i) {
					mBytes.push_back(location[i]);
				}
			}

			~MemoryState() {
				for(auto i = 0; i < mSize; ++i) {
					mLocation[i] = mBytes.at(i);
				}

				Platform::SetPageProtect(mLocation, mSize, mProtection);
			}
		};

		//this is platform and arch dependent right now, need to add support for others in the future 
		bool Create(void* function, void* funcJump, void* funcTrampoline) {
			bool successful = false;

			if(Platform::Type == PlatformType::Windows) {
				u8* u8Function = reinterpret_cast<u8*>(function);

				// todo: use capstone or zydis
				if(*u8Function == 0xff && *(u8Function + 1) == 0x25) {
					uintptr_t* address = reinterpret_cast<uintptr_t*>(u8Function + (*reinterpret_cast<u32*>(u8Function + 2)) + 6);

					successful = Platform::CreateHook(reinterpret_cast<void*>(*address), funcJump, reinterpret_cast<void**>(funcTrampoline));
				}
			}

			return successful;
		}

		bool Create(void* vtable, void* function, void* funcJump, void* funcTrampoline) {
			bool successful = false;

			if(Platform::Type == PlatformType::Windows) {
				u8* thunkFunction = reinterpret_cast<u8*>(function);
				if (*thunkFunction == 0xe9) {
					function = reinterpret_cast<void*>(thunkFunction + (*reinterpret_cast<u32*>(thunkFunction + 1)) + 5);
				}

				u8* u8Function = reinterpret_cast<u8*>(function) + 3;

				if(*u8Function == 0xff) {
					u8* offsetVtable = reinterpret_cast<u8*>(vtable);
					void* address = nullptr;

					u8 modrm = *(u8Function + 1);
					if (modrm == 0x60) {
						address = *reinterpret_cast<void**>(offsetVtable + *reinterpret_cast<u8*>(u8Function + 2));
					}
					else if (modrm == 0xA0) {
						address = *reinterpret_cast<void**>(offsetVtable + *reinterpret_cast<u32*>(u8Function + 2));
					}

					successful = Platform::CreateHook(address, funcJump, reinterpret_cast<void**>(funcTrampoline));
				}
			}

			return successful;
		}
	}
}