#include "JsonHelper.h"

#include "Zenova/Log.h"

namespace Zenova {
    json::Document JsonHelper::OpenFile(const std::string& fileLocation, bool missingFile) {
        json::Document d;
        std::ifstream ifs(fileLocation);
        if(ifs.is_open()) {
            d.ParseStream(json::IStreamWrapper(ifs));
        }
        else if(missingFile) {
            Zenova_Warn("\"" + fileLocation + "\" not found");
        }

        return d;
    }

    const json::Value& JsonHelper::FindMember(const json::Value& obj, const std::string& memberStr) {
        auto& objIter = obj.FindMember(memberStr.c_str());
        if(objIter != obj.MemberEnd()) {
            return objIter->value;
        }

        Zenova_Warn("Couldn't find " + memberStr);
        static const json::Value null;
        return null;
    }

    std::string JsonHelper::FindString(const json::Value& rootObject, const std::string& memberStr) {
        auto& objStr = JsonHelper::FindMember(rootObject, memberStr);
        if(!objStr.IsNull()) {
            if(objStr.IsString()) {
                return objStr.GetString();
            }
            else {
                Zenova_Warn(memberStr + " isn't a String");
            }
        }

        return "";
    }
}