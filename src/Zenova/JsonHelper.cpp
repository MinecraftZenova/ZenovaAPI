#include "JsonHelper.h"

#include <filesystem>

#include "Globals.h"
#include "Zenova/Log.h"

namespace Zenova {
    json::Document JsonHelper::OpenFile(const std::string& fileLocation, bool missingFile) {
        json::Document d;
        std::ifstream ifs(fileLocation);

        if (ifs.is_open()) {
            d.ParseStream(json::IStreamWrapper(ifs));
        }
        else if (missingFile) {
            logger.warn("\"{}\" not found", fileLocation);
        }

        return d;
    }

    const json::Value& JsonHelper::FindMember(const json::Value& obj, const std::string& memberStr, bool missingMember) {
        auto& objIter = obj.FindMember(memberStr.c_str());
        if (objIter != obj.MemberEnd()) {
            return objIter->value;
        }

        if (missingMember) {
            logger.warn("Couldn't find {}", memberStr);
        }

        static const json::Value null;
        return null;
    }

    std::string JsonHelper::FindString(const json::Value& rootObject, const std::string& memberStr, bool missingMember) {
        auto& objStr = JsonHelper::FindMember(rootObject, memberStr, missingMember);
        if(!objStr.IsNull()) {
            if(objStr.IsString()) {
                return objStr.GetString();
            }
            else {
                logger.warn("{} isn't a String", memberStr);
            }
        }

        return "";
    }
}