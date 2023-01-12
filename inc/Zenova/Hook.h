#pragma once

#include <string>
#include <unordered_map>
#include <tuple>
#include <vector> //std::vector
#include <utility> //std::pair
#include <type_traits>
#include <iostream>

#include "Common.h"
#include "Log.h"
#include "Platform.h"

// there's one too many overloaded versions of the create function
#define _ZENOVA_HOOK_CREATE_EXTRA \
	void* funcJump, void* funcTrampoline, const char* funcName = nullptr

namespace Zenova {
	namespace Hook {
		EXPORT uintptr_t SlideAddress(std::size_t offset);
		EXPORT std::size_t UnslideAddress(uintptr_t result);

		// virtualDtor = address of the generated dtor in the vtable (msvc)
		EXPORT uintptr_t GetRealDtor(uintptr_t virtualDtor);
		EXPORT uintptr_t Sigscan(const char* sig, const char* mask);
		EXPORT uintptr_t SigscanCall(const char* sig, const char* mask);

		EXPORT bool Create(void* function, _ZENOVA_HOOK_CREATE_EXTRA);
		EXPORT bool Create(void* vtable, void* function, _ZENOVA_HOOK_CREATE_EXTRA);

		template <typename T,
			std::enable_if_t<std::is_function<typename std::remove_pointer<T>::type>::value>* = nullptr>
		bool Create(T function, _ZENOVA_HOOK_CREATE_EXTRA) {
			return Create(*reinterpret_cast<void**>(&function), funcJump, funcTrampoline, funcName);
		}

		template <typename T,
			std::enable_if_t<std::is_member_function_pointer<typename std::remove_pointer<T>::type>::value>* = nullptr>
		bool Create(T function, _ZENOVA_HOOK_CREATE_EXTRA) {
			return Create(*reinterpret_cast<void**>(&function), funcJump, funcTrampoline, funcName);
		}

		template <typename T,
			std::enable_if_t<std::is_member_function_pointer<typename std::remove_pointer<T>::type>::value>* = nullptr>
		bool Create(void* vtable, T function, _ZENOVA_HOOK_CREATE_EXTRA) {
			return Create(vtable, *reinterpret_cast<void**>(&function), funcJump, funcTrampoline, funcName);
		}
	};
}

// defines the cast to choose an overloaded member function
// ex: Zenova_OCast(void, Test, int)(&Test::overload); chooses void Test::overload(int)
#define Zenova_OCast(r, c, ...) static_cast<r (c::*)(__VA_ARGS__)>

// ex: Zenova_Hook(Test::func, &func, &_func);
#define Zenova_Hook(function, hook, trampoline) \
	Zenova::Hook::Create(&function, hook, trampoline, #function)

// ex: Zenova_VHook(Test, vfunc, &func, &_func); hooks Test::Vfunc
#define Zenova_VHook(classname, function, hook, trampoline, ...) \
	Zenova::Hook::Create(classname##_vtable, (__VA_ARGS__(&classname::function)), hook, trampoline, #classname "::" #function)