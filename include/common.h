#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <optional>

// 常用类型别名
using String = std::string;
using StringVector = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;
using TimePoint = std::chrono::system_clock::time_point;

// 用户角色枚举
enum class UserRole {
    STUDENT = 0,
    CLASS_MONITOR = 1,    // 班长
    STUDY_MONITOR = 2,    // 学习委员
    LIFE_MONITOR = 3,     // 生活委员
    SPORTS_MONITOR = 4,   // 体育委员
    TEACHER = 5,
    ADMIN = 6
};

// 通知类型枚举
enum class NotificationType {
    GENERAL = 0,          // 普通通知
    URGENT = 1,           // 紧急通知
    ACTIVITY = 2,         // 活动通知
    ACADEMIC = 3,         // 学术通知
    FINANCE = 4           // 财务通知
};

// 请假状态枚举
enum class LeaveStatus {
    PENDING = 0,          // 待审批
    APPROVED = 1,         // 已批准
    REJECTED = 2,         // 已拒绝
    CANCELLED = 3         // 已取消
};

// 活动状态枚举
enum class ActivityStatus {
    PLANNING = 0,         // 计划中
    REGISTERING = 1,      // 报名中
    ONGOING = 2,          // 进行中
    COMPLETED = 3,        // 已完成
    CANCELLED = 4         // 已取消
};

// 通用返回结果结构
struct Result {
    bool success;
    std::string message;

    Result(bool s = false, const std::string& m = "")
        : success(s), message(m) {}

    static Result Success(const std::string& msg = "操作成功") {
        return Result(true, msg);
    }

    static Result Failure(const std::string& msg = "操作失败") {
        return Result(false, msg);
    }
};

// 分页信息结构
struct Pagination {
    int page = 1;
    int page_size = 20;
    int total = 0;
    int total_pages = 0;

    Pagination(int p = 1, int ps = 20) : page(p), page_size(ps) {}
};

// 时间工具函数
namespace TimeUtils {
    std::string formatTime(const TimePoint& tp);
    TimePoint parseTime(const std::string& timeStr);
    std::string getCurrentTimeString();
    TimePoint getCurrentTime();
}

// 字符串工具函数
namespace StringUtils {
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
}

// 加密工具函数
namespace CryptoUtils {
    std::string md5(const std::string& input);
    std::string sha256(const std::string& input);
    std::string generateSalt();
    std::string hashPassword(const std::string& password, const std::string& salt);
    bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt);
}

#endif // COMMON_H