#ifndef CRAAM_CONFIGURATIONEXECUTOR_HPP
#define CRAAM_CONFIGURATIONEXECUTOR_HPP

#include <string>
#include "Configuration.hpp"
#include "Result.hpp"
#include "algorithms/ValueIteration.hpp"
#include "easylogging++.h"

namespace craam {

class ConfigurationExecutor {
public:
    static Result executeConfiguration(const Configuration& configuration, const std::string& resourcesDir) {
        if (!configuration.hasMember(ALGORITHM_NAME)) {
            LOG(ERROR) << "Algorithm name not found." << std::endl;
            return Result(configuration, "Missing: algorithmName");
        }

        std::string algorithmName{configuration.getString(DOMAIN_NAME)};

        if (algorithmName == DOMAIN_GRID_WORLD) {
            return executeAlgorithm<GridWorld>(configuration, resourcesDir);
        } else if (algorithmName == DOMAIN_TRAFFIC) {
            return executeAlgorithm<Traffic>(configuration, resourcesDir);
        } else if (algorithmName == DOMAIN_TILES) {
            return executeAlgorithm<SlidingTilePuzzle>(configuration, resourcesDir);
        } else {
            const std::string& errorMessage = "Unknown: algorithmName: " + algorithmName;
            LOG(ERROR) << errorMessage << std::endl;
            return Result(configuration, errorMessage);
        }
    }

    template <typename Algorithm>
    static Result executeAlgorithm(const Configuration& configuration, const std::string& resourcesDir) {
        
        
        
    }
};
}

#endif // CRAAM_CONFIGURATIONEXECUTOR_HPP
