#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};

class Logger {
public:
    static Logger& getInstance();

    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filename);
    void enableConsoleOutput(bool enable);

    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

    template<typename... Args>
    void debug(const std::string& format, Args... args) {
        debug(formatString(format, args...));
    }

    template<typename... Args>
    void info(const std::string& format, Args... args) {
        info(formatString(format, args...));
    }

    template<typename... Args>
    void warning(const std::string& format, Args... args) {
        warning(formatString(format, args...));
    }

    template<typename... Args>
    void error(const std::string& format, Args... args) {
        error(formatString(format, args...));
    }

    template<typename... Args>
    void fatal(const std::string& format, Args... args) {
        fatal(formatString(format, args...));
    }

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string getCurrentTime();
    std::string getLevelString(LogLevel level);
    void writeLog(LogLevel level, const std::string& message);

    template<typename... Args>
    std::string formatString(const std::string& format, Args... args) {
        int size = snprintf(nullptr, 0, format.c_str(), args...);
        std::string result(size + 1, '\0');
        snprintf(&result[0], result.size(), format.c_str(), args...);
        result.pop_back(); // Remove null terminator
        return result;
    }

    LogLevel logLevel_;
    std::ofstream logFile_;
    std::mutex mutex_;
    bool consoleOutput_;
    std::string logFilename_;
};

// 宏定义简化日志调用
#define LOG_DEBUG(message) Logger::getInstance().debug(message)
#define LOG_INFO(message) Logger::getInstance().info(message)
#define LOG_WARNING(message) Logger::getInstance().warning(message)
#define LOG_ERROR(message) Logger::getInstance().error(message)
#define LOG_FATAL(message) Logger::getInstance().fatal(message)

#define LOG_DEBUG_FMT(format, ...) Logger::getInstance().debug(format, __VA_ARGS__)
#define LOG_INFO_FMT(format, ...) Logger::getInstance().info(format, __VA_ARGS__)
#define LOG_WARNING_FMT(format, ...) Logger::getInstance().warning(format, __VA_ARGS__)
#define LOG_ERROR_FMT(format, ...) Logger::getInstance().error(format, __VA_ARGS__)
#define LOG_FATAL_FMT(format, ...) Logger::getInstance().fatal(format, __VA_ARGS__)

#endif // LOGGER_H