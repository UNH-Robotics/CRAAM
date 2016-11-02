#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Configuration.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace craam {
class Result {
public:
    Result(const Configuration& configuration, std::string errorMessage)
            : configuration{configuration},
              errorMessage{std::move(errorMessage)},
              success{false},
              expandedNodes{0},
              generatedNodes{0},
              planningTime{0},
              pathLength{0},
              actions{std::vector<std::string>()} {};

    Result(const Configuration& configuration,
            const int expandedNodes,
            const int generatedNodes,
            const long long planningTime,
            const long long actionExecutionTime,
            const long long goalAchievementTime,
            const long long idlePlanningTime,
            const long long pathLength,
            const std::vector<std::string> actions)
            : configuration{configuration},
              errorMessage{""},
              success{true},
              expandedNodes{expandedNodes},
              generatedNodes{generatedNodes},
              planningTime{planningTime},
              pathLength{pathLength},
              actions{actions} {};

    std::string getJsonString() const {
        using namespace rapidjson;

        Document resultDocument;
        resultDocument.SetObject();

        auto& allocator = resultDocument.GetAllocator();

        //        resultDocument.AddMember("configuration", Value{configuration}, allocator);

        Value errorMessageValue;
        errorMessageValue.SetString(errorMessage.c_str(), errorMessage.size());
        resultDocument.AddMember("errorMessage", errorMessageValue, allocator);

        resultDocument.AddMember("success", success, allocator);
        resultDocument.AddMember("expandedNodes", Value{}.SetInt(expandedNodes), allocator);
        resultDocument.AddMember("generatedNodes", Value{}.SetInt(generatedNodes), allocator);
        resultDocument.AddMember("planningTime", Value{}.SetInt64(planningTime), allocator);
        resultDocument.AddMember("pathLength", Value{}.SetInt64(pathLength), allocator);

        //        GenericValue<ASCII<>> actionsArray{kArrayType};
        //        for (std::string action : actions) {
        //            actionsArray.PushBack(GenericValue<ASCII<>>{}.SetString(action.c_str(), action.size(), allocator),
        //            allocator);
        //        }
        //
        //        resultDocument.AddMember("actions", actionsArray, allocator);

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        resultDocument.Accept(writer);
        return buffer.GetString();
    }

    const Configuration& configuration;
    const std::string errorMessage;
    const bool success;
    const int expandedNodes;
    const int generatedNodes;
    const long long planningTime;
    const long long reward;
    const long long pathLength;
    const std::vector<std::string> actions;
};
}