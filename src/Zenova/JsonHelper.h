#pragma once

#include <string>
#include <fstream>

#ifdef _WIN32
#undef GetObject
#endif

#include "../rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
namespace json = rapidjson;

namespace Zenova {
	class JsonHelper {
	public:
		static json::Document OpenFile(const std::string& fileLocation);
		static const json::Value& FindMember(const json::Value& obj, const std::string& memberStr);
		static std::string FindString(const json::Value& rootObject, const std::string& memberStr);
	};
}