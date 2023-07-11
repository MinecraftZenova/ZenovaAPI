#pragma once

#include "Common.h"
#include "Platform.h"

#include <unordered_map>

namespace Zenova {
	namespace Hook {
		struct Vtable {
			uintptr_t address;
			std::unordered_map<std::string, size_t> funcOffsets;
		};

		ZENOVA_EXPORT uintptr_t SlideAddress(size_t offset);
		ZENOVA_EXPORT size_t UnslideAddress(uintptr_t result);

		ZENOVA_EXPORT uintptr_t* GetSymbol(const char* function);
		ZENOVA_EXPORT Vtable* GetVtable(const char* vtable);

		ZENOVA_EXPORT uintptr_t Sigscan(const char* sig, const char* mask, const char* funcName = nullptr);
		ZENOVA_EXPORT uintptr_t SigscanCall(const char* sig, const char* mask, const char* funcName = nullptr);

		ZENOVA_EXPORT bool Create(const char* function, void* funcJump, void* funcTrampoline);
		ZENOVA_EXPORT bool Create(uintptr_t address, void* funcJump, void* funcTrampoline, const char* funcName = nullptr);

		ZENOVA_EXPORT bool ReplaceVtable(const char* vtable, const char* function, void* funcJump, void* funcTrampoline);
		ZENOVA_EXPORT bool ReplaceVtable(uintptr_t vtable, size_t offset, void* funcJump, void* funcTrampoline, const char* funcName = nullptr);
	};
}

// ex: Zenova_Hook(Test::func, &func, &_func);
#define Zenova_Hook(function, hook, trampoline) \
	Zenova::Hook::Create(#function, hook, trampoline)

#define Zenova_VReplace(vtable, function, hook, trampoline) \
	Zenova::Hook::ReplaceVtable(#vtable, #vtable "::" #function, hook, trampoline)