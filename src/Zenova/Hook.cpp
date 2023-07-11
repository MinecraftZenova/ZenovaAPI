#define LOG_CONTEXT "Zenova::Hook"

#include "Zenova/Log.h"
#include "Zenova/Hook.h"

#include <list>

namespace Zenova::Hook {
	namespace {
		std::unordered_map<std::string, uintptr_t> symbols;
		std::unordered_map<std::string, Vtable> vtables;
	}

	uintptr_t SlideAddress(size_t offset) {
		return Platform::GetModuleBaseAddress() + offset;
	}

	uintptr_t UnslideAddress(uintptr_t result) {
		return result - Platform::GetModuleBaseAddress();
	}

	uintptr_t* GetSymbol(const char* function) {
		return &symbols[function];
	}

	Vtable* GetVtable(const char* vtable) {
		return &vtables[vtable];
	}

	bool MemCompare(const char* data, const char* sig, const char* mask) {
		for (; *mask; ++mask, ++data, ++sig) {
			if (*mask == 'x' && *data != *sig) {
				return false;
			}
		}

		return true;
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

	bool _Create(uintptr_t address, void* funcJump, void* funcTrampoline) {
		return Platform::CreateHook(reinterpret_cast<void*>(address), funcJump, funcTrampoline);
	}

	bool Create(const char* function, void* funcJump, void* funcTrampoline) {
		bool successful = false;
		uintptr_t address = *GetSymbol(function);
		if (address != 0) {
			successful = _Create(address, funcJump, funcTrampoline);
		}

		if (!successful) {
			Zenova_Error("Hook failed with function {}", function);
		}

		return successful;
	}

	bool Create(uintptr_t address, void* funcJump, void* funcTrampoline, const char* funcName) {
		bool successful = false;

		successful = _Create(SlideAddress(address), funcJump, funcTrampoline);

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

	bool ReplaceVtable(uintptr_t vtable, size_t offset, void* funcJump, void* funcTrampoline, const char* funcName) {
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

	bool ReplaceVtable(const char* vtable, const char* function, void* funcJump, void* funcTrampoline) {
		bool successful = false;
		
		const Vtable& v = *GetVtable(vtable);
		if (v.address != 0) {
			auto f = v.funcOffsets.find(function);
			if (f != v.funcOffsets.end()) {
				ReplaceVtable(v.address, f->second, funcJump, funcTrampoline, function);
			}
		}

		return false;
	}
}
