#include "../../include/common.h"
#include <iomanip>
#include <sstream>

namespace TimeUtils {

std::string formatTime(const TimePoint& tp) {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

TimePoint parseTime(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);

    // 尝试解析标准格式: YYYY-MM-DD HH:MM:SS
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        // 尝试解析日期格式: YYYY-MM-DD
        ss.clear();
        ss.str(timeStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
    }

    auto time_t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t);
}

std::string getCurrentTimeString() {
    return formatTime(getCurrentTime());
}

TimePoint getCurrentTime() {
    return std::chrono::system_clock::now();
}

} // namespace TimeUtils