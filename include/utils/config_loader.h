#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "../common.h"
#include <fstream>
#include <nlohmann/json.hpp>

class ConfigLoader {
public:
    static ConfigLoader& getInstance();

    // 加载配置文件
    bool loadConfig(const std::string& configFile);
    bool loadConfigFromString(const std::string& configJson);

    // 获取配置值
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;

    // 获取嵌套配置
    nlohmann::json getSection(const std::string& sectionName) const;

    // 检查配置是否存在
    bool hasKey(const std::string& key) const;

    // 获取所有配置
    nlohmann::json getAllConfig() const;

    // 重新加载配置
    bool reload();

    // 保存配置到文件
    bool saveConfig(const std::string& configFile) const;

private:
    ConfigLoader();
    ~ConfigLoader();
    ConfigLoader(const ConfigLoader&) = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

    nlohmann::json config_;
    std::string configFilePath_;
    std::time_t lastModified_;

    // 解析嵌套键
    std::vector<std::string> parseKey(const std::string& key) const;

    // 获取嵌套值
    nlohmann::json getNestedValue(const std::vector<std::string>& keys) const;

    // 检查文件修改时间
    bool isConfigModified() const;
};

// 配置管理器单例
#define CONFIG ConfigLoader::getInstance()

// 便捷的配置获取宏
#define CONFIG_STRING(key, default) CONFIG.getString(key, default)
#define CONFIG_INT(key, default) CONFIG.getInt(key, default)
#define CONFIG_DOUBLE(key, default) CONFIG.getDouble(key, default)
#define CONFIG_BOOL(key, default) CONFIG.getBool(key, default)

#endif // CONFIG_LOADER_H