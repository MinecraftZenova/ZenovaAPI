#pragma once

#include <string>
#include <unordered_map>

#include "Common.h"
#include "Log.h"
#include "OS/OS.h"

#ifdef _MSC_VER
	#define FSIG __FUNCSIG__
#else //gcc
	#define FSIG __PRETTY_FUNCTION__
#endif

class ZenovaConstructor {
public:	
	ZenovaConstructor(uintptr_t* originalVtable);
};

namespace Zenova {
	namespace Hook {
		extern uintptr_t BaseAddress;
		extern std::unordered_map<std::string, uintptr_t> Symbols; //Change this to a class/namespace/global map -> function map 
		
		EXPORT uintptr_t SlideAddress(size_t offset); //only meant for internal use
		EXPORT uintptr_t UnslideAddress(uintptr_t result);
		EXPORT uintptr_t FindSymbol(const char* scope, const char* funcName); //Use "global" for a global scope

		template<typename T, typename... Targs>
		T Call(uintptr_t func, Targs&&... args) {
			return (reinterpret_cast<T (*)(Targs...)>(SlideAddress(func)))(std::forward<Targs>(args)...);
		}

		template<typename T, typename... Targs>
		T Call(const std::string& func, Targs&&... args) {
			return Call<T>(FindSymbol("", func.c_str()), args...);
		}

		//TODO: Handle cases where the user implements the first virtual function
		//(Should be able to do by making sure there's no virtual functions from the "template" class who's vtable we override
		template<typename T, typename = std::enable_if_t<std::is_class<T>::value>>
		bool RepairVtable(uintptr_t* vtable, uintptr_t* origVtable) {
			size_t size = hostOS->GetVtableSize(reinterpret_cast<uintptr_t**>(origVtable)); //should return 111 w/ the original Item vtable
			if(size == 0) { //prevent replacing a non existant vtable or one that breaks the rule
				return false;
			}

			u32 old = hostOS->SetPageProtect(vtable, size, hostOS->GetRWProtect());
			if(old == 0) {
				return false;
			}

			T obj(ZenovaConstructor());
			uintptr_t* tVtable = *reinterpret_cast<uintptr_t**>(&obj);
			for(size_t i = 0; i < size; i++) {
				if(vtable[i] == tVtable[i]) {
					vtable[i] = origVtable[i];
				}
			}
		
			hostOS->SetPageProtect(vtable, size, old);

			return true;
		}
		//I'll have to adjust this for classes we make that inherit multiple vtables
	};
}