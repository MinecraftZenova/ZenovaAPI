#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#ifdef ZENOVA_API
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport) 
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

//move to a Util header?
namespace Zenova {
	namespace Util {
		std::wstring StrToWstr(const std::string& oldstr);
	}
}

//this is convienant, idc about the risks
//implementation in Log.cpp
namespace std {
	class tstring : public wstring {
	public:
		string strData;

		using wstring::wstring;
		tstring(const string&);
		tstring(const char*);
		tstring(const wstring&);

		const string& str();
	};
}