#include "../../include/utils/config_loader.h"
#include "../../include/utils/logger.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

ConfigLoader::ConfigLoader() : configFilePath_("") {
    LOG_INFO("ConfigLoader initialized");
}

ConfigLoader::~ConfigLoader() {
    LOG_INFO("ConfigLoader destroyed");
}

ConfigLoader& ConfigLoader::getInstance() {
    static ConfigLoader instance;
    return instance;
}

bool ConfigLoader::loadConfig(const std::string& configFile) {
    try {
        std::ifstream file(configFile);
        if (!file.is_open()) {
            LOG_ERROR_FMT("Failed to open config file: %s", configFile.c_str());
            return false;
        }

        // 解析JSON
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        file.close();

        config_ = jsonConfig;
        configFilePath_ = configFile;

        // 记录文件修改时间
        struct stat fileStat;
        if (stat(configFile.c_str(), &fileStat) == 0) {
            lastModified_ = fileStat.st_mtime;
        }

        LOG_INFO_FMT("Config loaded successfully from: %s", configFile.c_str());
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Failed to load config file %s: %s", configFile.c_str(), e.what());
        return false;
    }
}

bool ConfigLoader::loadConfigFromString(const std::string& configJson) {
    try {
        config_ = nlohmann::json::parse(configJson);
        configFilePath_ = ""; // 字符串配置没有文件路径
        LOG_INFO("Config loaded from string successfully");
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Failed to parse config JSON: %s", e.what());
        return false;
    }
}

std::string ConfigLoader::getString(const std::string& key, const std::string& defaultValue) const {
    try {
        auto keys = parseKey(key);
        auto value = getNestedValue(keys);

        if (value.is_null()) {
            return defaultValue;
        }

        if (value.is_string()) {
            return value.get<std::string>();
        } else {
            return value.dump();
        }

    } catch (const std::exception& e) {
        LOG_WARNING_FMT("Failed to get string config '%s': %s, using default: %s",
                       key.c_str(), e.what(), defaultValue.c_str());
        return defaultValue;
    }
}

int ConfigLoader::getInt(const std::string& key, int defaultValue) const {
    try {
        auto keys = parseKey(key);
        auto value = getNestedValue(keys);

        if (value.is_null()) {
            return defaultValue;
        }

        if (value.is_number_integer()) {
            return value.get<int>();
        } else if (value.is_string()) {
            return std::stoi(value.get<std::string>());
        } else {
            return defaultValue;
        }

    } catch (const std::exception& e) {
        LOG_WARNING_FMT("Failed to get int config '%s': %s, using default: %d",
                       key.c_str(), e.what(), defaultValue);
        return defaultValue;
    }
}

double ConfigLoader::getDouble(const std::string& key, double defaultValue) const {
    try {
        auto keys = parseKey(key);
        auto value = getNestedValue(keys);

        if (value.is_null()) {
            return defaultValue;
        }

        if (value.is_number_float()) {
            return value.get<double>();
        } else if (value.is_number_integer()) {
            return static_cast<double>(value.get<int>());
        } else if (value.is_string()) {
            return std::stod(value.get<std::string>());
        } else {
            return defaultValue;
        }

    } catch (const std::exception& e) {
        LOG_WARNING_FMT("Failed to get double config '%s': %s, using default: %f",
                       key.c_str(), e.what(), defaultValue);
        return defaultValue;
    }
}

bool ConfigLoader::getBool(const std::string& key, bool defaultValue) const {
    try {
        auto keys = parseKey(key);
        auto value = getNestedValue(keys);

        if (value.is_null()) {
            return defaultValue;
        }

        if (value.is_boolean()) {
            return value.get<bool>();
        } else if (value.is_string()) {
            std::string str = value.get<std::string>();
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            return (str == "true" || str == "1" || str == "yes" || str == "on");
        } else if (value.is_number()) {
            return value.get<int>() != 0;
        } else {
            return defaultValue;
        }

    } catch (const std::exception& e) {
        LOG_WARNING_FMT("Failed to get bool config '%s': %s, using default: %s",
                       key.c_str(), e.what(), defaultValue ? "true" : "false");
        return defaultValue;
    }
}

nlohmann::json ConfigLoader::getSection(const std::string& sectionName) const {
    try {
        auto keys = parseKey(sectionName);
        auto value = getNestedValue(keys);

        if (value.is_null()) {
            return nlohmann::json::object();
        }

        return value;

    } catch (const std::exception& e) {
        LOG_WARNING_FMT("Failed to get config section '%s': %s", sectionName.c_str(), e.what());
        return nlohmann::json::object();
    }
}

bool ConfigLoader::hasKey(const std::string& key) const {
    try {
        auto keys = parseKey(key);
        auto value = getNestedValue(keys);
        return !value.is_null();

    } catch (const std::exception& e) {
        return false;
    }
}

nlohmann::json ConfigLoader::getAllConfig() const {
    return config_;
}

bool ConfigLoader::reload() {
    if (configFilePath_.empty()) {
        LOG_WARNING("No config file path set, cannot reload");
        return false;
    }

    if (isConfigModified()) {
        LOG_INFO_FMT("Config file modified, reloading: %s", configFilePath_.c_str());
        return loadConfig(configFilePath_);
    }

    return true;
}

bool ConfigLoader::saveConfig(const std::string& configFile) const {
    try {
        std::ofstream file(configFile);
        if (!file.is_open()) {
            LOG_ERROR_FMT("Failed to open config file for writing: %s", configFile.c_str());
            return false;
        }

        file << config_.dump(2); // 使用2个空格的缩进
        file.close();

        LOG_INFO_FMT("Config saved successfully to: %s", configFile.c_str());
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Failed to save config file %s: %s", configFile.c_str(), e.what());
        return false;
    }
}

std::vector<std::string> ConfigLoader::parseKey(const std::string& key) const {
    std::vector<std::string> keys;
    std::stringstream ss(key);
    std::string token;

    while (std::getline(ss, token, '.')) {
        keys.push_back(token);
    }

    return keys;
}

nlohmann::json ConfigLoader::getNestedValue(const std::vector<std::string>& keys) const {
    nlohmann::json current = config_;

    for (const auto& key : keys) {
        if (current.is_object() && current.contains(key)) {
            current = current[key];
        } else {
            return nlohmann::json(); // 返回null
        }
    }

    return current;
}

bool ConfigLoader::isConfigModified() const {
    if (configFilePath_.empty()) {
        return false;
    }

    struct stat fileStat;
    if (stat(configFilePath_.c_str(), &fileStat) == 0) {
        return fileStat.st_mtime > lastModified_;
    }

    return false;
}