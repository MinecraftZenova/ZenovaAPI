#include "Zenova/Hook.h"

#include <list>

namespace Zenova {
	uintptr_t Hook::BaseAddress = 0;
	std::unordered_map<std::string, uintptr_t> Hook::Symbols;
		
	uintptr_t Hook::SlideAddress(size_t offset) {
		return BaseAddress + offset;
	}

	uintptr_t Hook::UnslideAddress(uintptr_t result) {
		return result - BaseAddress;
	}
	
	uintptr_t Hook::FindSymbol(const char* scope, const char* funcName) {
		//auto iter = Symbols.find(scope);
		//
		//if(iter == Symbols.end()) {
		//	Log::info("FindSymbol" , std::string("Scope ") + scope + "not used"); 
		//}
		//else {
			auto iter = Symbols.find(funcName);

			if(iter == Symbols.end()) {
				Info(std::string("Function ") + funcName + "not found");
			}
			else {
				return iter->second;
			}
		//}
		return 0;
	}
}