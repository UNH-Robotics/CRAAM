#ifndef CRAAM_CONFIGURATION_HPP
#define CRAAM_CONFIGURATION_HPP

#include <rapidjson/istreamwrapper.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/istreamwrapper.h"

namespace craam {

static const std::string RAW_DOMAIN{"rawDomain"};
static const std::string DOMAIN_NAME{"domainName"};
static const std::string DOMAIN_PATH{"domainPath"};
static const std::string DOMAIN_INSTANCE{"domainInstanceName"};
static const std::string ALGORITHM_NAME{"algorithmName"};
static const std::string TERMINATION_CHECKER_TYPE{"terminationType"};
static const std::string ACTION_DURATION{"actionDuration"};
static const std::string TIME_LIMIT{"timeLimit"};
static const std::string LOOKAHEAD_TYPE{"lookaheadType"};

static const std::string DOMAIN_GRID_WORLD{"MULTI_ARM_BANDIT"};

static const std::string ALGORITHM_VI{"VI"};
static const std::string ALGORITHM_UCB{"UCB"};
static const std::string ALGORITHM_THOMPSON_SAMPLING{"THOMPSON_SAMPLING"};

static const std::string TERMINATION_CHECKER_TIME{"TIME"};
static const std::string TERMINATION_CHECKER_EXPANSION{"EXPANSION"};

static const std::string LOOKAHEAD_STATIC{"STATIC"};
static const std::string LOOKAHEAD_DYNAMIC{"DYNAMIC"};

class Configuration {
public:
    static Configuration fromFile(const std::string& configurationPath) {
        rapidjson::Document document;

        if (!fileExists(configurationPath)) {
            std::cerr << "Invalid configuration file: " << configurationPath << std::endl;
        }

        std::ifstream configurationFile{configurationPath};
        rapidjson::IStreamWrapper streamWrapper{configurationFile};
        document.ParseStream(streamWrapper);

        return Configuration(std::move(document));
    }

    static Configuration fromStream(std::istream& stream) {
        rapidjson::Document document;
        std::stringstream jsonStream;

        for (std::string line; std::getline(stream, line);) {
            if (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
                break; // Terminate paring on empty line
            }

            jsonStream << line;
        }

        rapidjson::IStreamWrapper streamWrapper{jsonStream};
        document.ParseStream(streamWrapper);
        return Configuration(std::move(document));
    }

    Configuration() : document{} {};
    Configuration(const Configuration&) = default;
    Configuration(Configuration&&) = default;

    Configuration(rapidjson::Document document) : document{std::move(document)} {}

    Configuration(const std::string& json) : document{} { document.Parse(json.c_str()); }

    bool hasMember(const std::string& key) const { return document.HasMember(key.c_str()); }

    std::string getString(const std::string& key) const { return std::string{document[key.c_str()].GetString()}; }
    long long int getLong(const std::string& key) const { return document[key.c_str()].GetInt64(); }

    std::string getStringOrThrow(const std::string& key) const {
        if (!hasMember(key)) {
            throw std::runtime_error("Invalid key: " + key);
        }

        return std::string{document[key.c_str()].GetString()};
    }

    long long int getLongOrThrow(const std::string& key) const {
        if (!hasMember(key)) {
            throw std::runtime_error("Invalid key: " + key);
        }

        return document[key.c_str()].GetInt64();
    }

private:
    static bool fileExists(const std::string& path) {
        struct stat buffer;
        return stat(path.c_str(), &buffer) != -1;
    }

    rapidjson::Document document;
};
}

#endif // METRONOME_CONFIGURATION_HPP
