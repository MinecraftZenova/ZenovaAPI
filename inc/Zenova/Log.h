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

		inline Log(const UniversalString& name) : mName(name) {}

		template<typename... Args>
		inline void write(Severity severity, const UniversalString& format, const Args&... args) {
			Write(severity, mName, format, args...);
		}

		template<typename... Args>
		inline void info(const UniversalString& format, const Args&... args) {
			Info(mName, format, args...);
		}
		template<typename... Args>
		inline void warn(const UniversalString& format, const Args&... args) {
			Warn(mName, format, args...);
		}

		template<typename... Args>
		inline void error(const UniversalString& format, const Args&... args) {
			Error(mName, format, args...);
		}

		template<typename... Args>
		static void Write(UniversalString severity, const UniversalString& name, const UniversalString& format, const Args&... message) {
			static std::mutex logMutex;
			std::lock_guard<std::mutex> guard(logMutex);

			std::wstring s = fmt::format(L"[{}] [{}] ", name.wstr, severity.wstr);
			s += fmt::format(format.wstr + L"\n", (VTStrToWstr(message))...);

			Platform::OutputDebugMessage(s);
			std::wcout << s << std::flush;
		}

		template<typename... Args>
		static void Write(Severity severity, const UniversalString& name, const UniversalString& format, const Args&... message) {
			Write(_serverityToString(severity), name, format, message...);
		}

		template<typename... Args>
		static void Info(const UniversalString& name, const UniversalString& format, const Args&... args) {
			Write(Severity::Info, name, format, args...);
		}

		template<typename... Args>
		static void Warn(const UniversalString& name, const UniversalString& format, const Args&... args) {
			Write(Severity::Warning, name, format, args...);
		}

		template<typename... Args>
		static void Error(const UniversalString& name, const UniversalString& format, const Args&... args) {
			Write(Severity::Error, name, format, args...);
		}
	
	protected:
		static std::string _serverityToString(Severity severity) {
			switch(severity) {
				case Severity::None: {
					return "";
				}
				case Severity::Info: {
					return "Info";
				}
				case Severity::Warning: {
					return "Warning";
				}
				case Severity::Error: {
					return "Error";
				}
				default: {
					return "Unknown";
				}
			}
		}
	};
}

#define Zenova_Info(f, ...) Zenova::Log::Info(FSIG, f, __VA_ARGS__)
#define Zenova_Warn(f, ...) Zenova::Log::Warn(FSIG, f, __VA_ARGS__)
#define Zenova_Error(f, ...) Zenova::Log::Error(FSIG, f, __VA_ARGS__)