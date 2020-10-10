#pragma once

#include <iostream>
#include <mutex>
#include <utility>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include "Common.h"
#include "Platform.h"

namespace Zenova {
	//maybe handle this in a different way, seems to work on C++11 -> C++17
	inline std::wstring VTStrToWstr(const std::string& str) { return std::wstring(str.begin(), str.end()); }
	inline std::wstring VTStrToWstr(std::string&& str) { return std::wstring(str.begin(), str.end()); }
	inline std::wstring VTStrToWstr(const char* str) { return VTStrToWstr(std::string(str)); }
	inline std::wstring VTStrToWstr(std::string_view str) { return VTStrToWstr(str.data()); }

	template<typename T>
	inline T VTStrToWstr(T&& ret) { return ret; }

	class Log {
		UniversalString mName;

	public:
		enum class Severity : int {
			None,
			Info,
			Warning,
			Error
		};

	protected:
		static const wchar_t* _serverityToString(Severity severity);

		template<typename... Args>
		static fmt::wformat_args ArgsHelper(const Args&... args) {
			return fmt::make_format_args<fmt::wformat_context>(args...);
		}

	public:
		Log(const UniversalString& name) : mName(name) {}

		static void Write(Severity severity, const UniversalString& name, const UniversalString& format, const fmt::wformat_args& args);

		template<typename... Args>
		static void Write(Severity severity, const UniversalString& name, const UniversalString& format, const Args&... args) {
			Write(severity, name, format, ArgsHelper((VTStrToWstr(args))...)); //this may run into lifetime issues
		}

		template<typename... Args>
		void write(Severity severity, const UniversalString& format, const Args&... args) {
			Write(severity, mName, format, args...);
		}

		template<typename... Args>
		void info(const UniversalString& format, const Args&... args) {
			Write(Severity::Info, mName, format, args...);
		}

		template<typename... Args>
		void warn(const UniversalString& format, const Args&... args) {
			Write(Severity::Warning, mName, format, args...);
		}

		template<typename... Args>
		void error(const UniversalString& format, const Args&... args) {
			Write(Severity::Error, mName, format, args...);
		}
	};
}

#define Zenova_Log(type, f, ...) Zenova::Log::Write(Zenova::Log::Severity::type, FSIG, f, __VA_ARGS__)
#define Zenova_Info(f, ...) Zenova_Log(Info, f, __VA_ARGS__)
#define Zenova_Warn(f, ...) Zenova_Log(Warning, f, __VA_ARGS__)
#define Zenova_Error(f, ...) Zenova_Log(Error, f, __VA_ARGS__)