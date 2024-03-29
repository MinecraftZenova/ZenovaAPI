#pragma once

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <string>

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

namespace Zenova {
	using exception = std::runtime_error;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	template<typename E, std::enable_if_t<std::is_enum<E>::value>* = nullptr>
	constexpr std::underlying_type_t<E> enum_cast(E enumValue) {
		return static_cast<std::underlying_type_t<E>>(enumValue);
	}

	//In the future add string operators?
	class EXPORT UniversalString {
		std::wstring s;

	public:
		UniversalString();
		UniversalString(const std::string& pstr);
		UniversalString(const std::wstring& pwstr);
		UniversalString(const char* pstr);
		UniversalString(const wchar_t* pwstr);

		std::string str() const;
		std::wstring wstr() const;
	};
}