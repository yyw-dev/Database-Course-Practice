#include "../../include/utils/logger.h"
#include <iostream>
#include <ctime>

Logger::Logger() : logLevel_(LogLevel::INFO), consoleOutput_(true) {
    logFilename_ = "app.log";
    logFile_.open(logFilename_, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << logFilename_ << std::endl;
    }
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    logLevel_ = level;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFilename_ = filename;
    logFile_.open(logFilename_, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << logFilename_ << std::endl;
    }
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    consoleOutput_ = enable;
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (level < logLevel_) {
        return;
    }

    std::string logEntry = "[" + getCurrentTime() + "] [" +
                          getLevelString(level) + "] " + message + "\n";

    if (consoleOutput_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << logEntry;
        } else {
            std::cout << logEntry;
        }
    }

    if (logFile_.is_open()) {
        logFile_ << logEntry;
        logFile_.flush();
    }
}

void Logger::debug(const std::string& message) {
    writeLog(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    writeLog(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    writeLog(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    writeLog(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    writeLog(LogLevel::FATAL, message);
}