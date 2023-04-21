#include "Zenova/Hook.h"

#include "Zenova/Globals.h"

#include <list>

namespace Zenova {
	namespace {
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

				for (auto i = 0; i < size; ++i) {
					mBytes.push_back(location[i]);
				}
			}

			~MemoryState() {
				for (auto i = 0; i < mSize; ++i) {
					mLocation[i] = mBytes.at(i);
				}

				Platform::SetPageProtect(mLocation, mSize, mProtection);
			}
		};
	}

	namespace Hook {
		uintptr_t SlideAddress(size_t offset) {
			return Platform::GetMinecraftBaseAddress() + offset;
		}

		uintptr_t UnslideAddress(uintptr_t result) {
			return result - Platform::GetMinecraftBaseAddress();
		}

		bool MemCompare(const char* data, const char* sig, const char* mask) {
			for (; *mask; ++mask, ++data, ++sig) {
				if (*mask == 'x' && *data != *sig) {
					return false;
				}
			}

			return true;
		}

		// doesn't work for inlined dtors, need to put an error message when there's a vtable load at the start
		uintptr_t GetRealDtor(uintptr_t virtualDtor) {
			virtualDtor = *reinterpret_cast<uintptr_t*>(virtualDtor); 
			u8* iaddr = reinterpret_cast<u8*>(virtualDtor);
			if (MemCompare(reinterpret_cast<const char*>(iaddr), "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x8B\xDA\x48\x8B\xF9", "xxxx?xxxxxxxxxx")) {
				return virtualDtor + 0x14 + *reinterpret_cast<int32_t*>(iaddr + 0x10);
			}

			Zenova_Error("0x{:x} is not a valid virtual dtor, dtor might be inlined?", virtualDtor);
			return 0;
		}

		// todo: figure out if we need to verify that all regions can be read
		uintptr_t Sigscan(const char* sig, const char* mask, const char* funcName) {
			uintptr_t size = Platform::GetMinecraftSize() - std::strlen(mask);

			for (size_t i = 0; i < size; ++i) {
				uintptr_t iaddr = SlideAddress(i);
				if (MemCompare(reinterpret_cast<const char*>(iaddr), sig, mask)) {
					return iaddr;
				}
			}

			if (funcName) {
				Zenova_Error("Sigscan failed with function: {}", funcName);
			}
			else {
				Zenova_Error("Sigscan failed with sig: {}", sig);
			}

			return 0;
		}

		uintptr_t SigscanCall(const char* sig, const char* mask, const char* funcName) {
			uintptr_t addr = Sigscan(sig, mask, funcName);

			if (addr != 0) {
				uintptr_t offset = addr + 1;
				addr = offset + 4 + *reinterpret_cast<int32_t*>(offset);
			} 

			return addr;
		}

		// this is platform and arch dependent right now, need to add support for others in the future 
		bool Create(void* function, void* funcJump, void* funcTrampoline, const char* funcName) {
			bool successful = false;

			if(Platform::Type == PlatformType::Windows) {
				u8* u8Function = reinterpret_cast<u8*>(function);

				// todo: use capstone or zydis
				if(*u8Function == 0xff && *(u8Function + 1) == 0x25) {
					uintptr_t* address = reinterpret_cast<uintptr_t*>(u8Function + (*reinterpret_cast<u32*>(u8Function + 2)) + 6);

					successful = Platform::CreateHook(reinterpret_cast<void*>(*address), funcJump, funcTrampoline);
				}
			}

			if (!successful) {
				if (funcName) {
					Zenova_Error("Hook failed with function {}", funcName);
				}
				else {
					Zenova_Error("Hook failed with function {}", function);
				}
			}

			return successful;
		}

		EXPORT bool Create(uintptr_t address, void* funcJump, void* funcTrampoline, const char* funcName) {
			bool successful = false;

			successful = Platform::CreateHook(reinterpret_cast<void*>(SlideAddress(address)), funcJump, funcTrampoline);

			if (!successful) {
				if (funcName) {
					Zenova_Error("Hook failed with function {}", funcName);
				}
				else {
					Zenova_Error("Hook failed with address {}", address);
				}
			}

			return successful;
		}

		bool VfuncHelper(size_t& outOffset, void* function) {
			u8* thunkFunction = reinterpret_cast<u8*>(function);
			if (*thunkFunction == 0xe9) {
				function = reinterpret_cast<void*>(thunkFunction + (*reinterpret_cast<u32*>(thunkFunction + 1)) + 5);
			}

			u8* u8Function = reinterpret_cast<u8*>(function) + 3;
			if (*u8Function == 0xff) {
				u8 modrm = *(u8Function + 1);
				if (modrm == 0x60) {
					outOffset = *reinterpret_cast<u8*>(u8Function + 2);
				}
				else if (modrm == 0xa0) {
					outOffset = *reinterpret_cast<u32*>(u8Function + 2);
				}

				return true;
			}

			return false;
		}

		bool Create(void* vtable, void* function, void* funcJump, void* funcTrampoline, const char* funcName) {
			bool successful = false;

			if(Platform::Type == PlatformType::Windows) {
				size_t offset;
				if (VfuncHelper(offset, function)) {
					u8* offsetVtable = reinterpret_cast<u8*>(vtable);
					void* address = *reinterpret_cast<void**>(offsetVtable + offset);

					successful = Platform::CreateHook(address, funcJump, funcTrampoline);
				}
			}

			if (!successful) {
				if (funcName) {
					Zenova_Error("VHook failed with function {}", funcName);
				}
				else {
					Zenova_Error("VHook failed with vtable {} and function {}", vtable, function);
				}
			}

			return successful;
		}

		bool ReplaceVtable(void* vtable, size_t offset, void* funcJump, void* funcTrampoline, const char* funcName) {
			bool successful = false;

			void** func = reinterpret_cast<void**>(reinterpret_cast<u8*>(vtable) + (offset * sizeof(void*)));

			u32 oldProtect = Platform::SetPageProtect(func, sizeof(void*), ProtectionFlags::Execute | ProtectionFlags::Write);

			if (funcTrampoline)
				*reinterpret_cast<void**>(funcTrampoline) = *func;

			*func = funcJump;
			successful = true;

			Platform::SetPageProtect(func, sizeof(void*), oldProtect);

			if (!successful) {
				if (funcName) {
					Zenova_Error("VReplace failed with function {}", funcName);
				}
				else {
					Zenova_Error("VReplace failed with vtable {} and offset {}", vtable, offset);
				}
			}

			return successful;
		}

		bool ReplaceVtable(void* vtable, void* function, void* funcJump, void* funcTrampoline, const char* funcName) {
			bool successful = false;
			
			size_t offset;
			if (VfuncHelper(offset, function)) {
				return ReplaceVtable(vtable, offset / sizeof(void*), funcJump, funcTrampoline, funcName);
			}

			return false;
		}
	}
}
