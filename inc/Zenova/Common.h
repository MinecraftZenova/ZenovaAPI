#pragma once

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <string>

#include <Utils/Utils.h>

//I honestly shouldn't do this
#pragma warning(disable:4251)

#ifdef ZENOVA_API
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport) 
#endif

#ifdef _MSC_VER
	#define FSIG __FUNCTION__
#else //gcc
	#define FSIG __func__
#endif

using exception = std::runtime_error;

//unsigned int types
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

//signed int types
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

template<typename E, std::enable_if_t<std::is_enum<E>::value>* = nullptr>
std::underlying_type_t<E> enum_cast(E enumValue) {
	return static_cast<std::underlying_type_t<E>>(enumValue);
}

namespace Zenova {
	//In the future add string operators?
	class UniversalString {
	public:
		std::wstring wstr;

		EXPORT UniversalString();
		EXPORT UniversalString(const std::string& pstr);
		EXPORT UniversalString(const std::wstring& pwstr);
		EXPORT UniversalString(const char* pstr);
		EXPORT UniversalString(const wchar_t* pwstr);

		std::string str() { return Util::WstrToStr(wstr); }
	};
}