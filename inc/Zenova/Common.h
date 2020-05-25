#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

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

namespace Zenova {
	//move to a Util header?
	namespace Util {
		std::string WstrToStr(const std::wstring& oldwstr);
		std::wstring StrToWstr(const std::string& oldstr);
	}

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