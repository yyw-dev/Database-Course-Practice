#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <regex>

class Validator {
public:
    // 用户名验证：字母数字下划线，3-20位
    static bool validateUsername(const std::string& username) {
        std::regex pattern("^[a-zA-Z0-9_]{3,20}$");
        return std::regex_match(username, pattern);
    }

    // 密码验证：至少8位，包含大小写字母和数字
    static bool validatePassword(const std::string& password) {
        if (password.length() < 8) return false;

        bool hasLower = false, hasUpper = false, hasDigit = false;
        for (char c : password) {
            if (std::islower(c)) hasLower = true;
            else if (std::isupper(c)) hasUpper = true;
            else if (std::isdigit(c)) hasDigit = true;
        }

        return hasLower && hasUpper && hasDigit;
    }

    // 邮箱验证
    static bool validateEmail(const std::string& email) {
        std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        return std::regex_match(email, pattern);
    }

    // 手机号验证（中国大陆）
    static bool validatePhone(const std::string& phone) {
        std::regex pattern("^1[3-9]\\d{9}$");
        return std::regex_match(phone, pattern);
    }

    // 学号验证：数字字母组合，6-20位
    static bool validateStudentId(const std::string& studentId) {
        std::regex pattern("^[a-zA-Z0-9]{6,20}$");
        return std::regex_match(studentId, pattern);
    }

    // 真实姓名验证：中文或英文姓名
    static bool validateRealName(const std::string& name) {
        if (name.empty() || name.length() > 50) return false;

        // 检查是否包含非法字符
        for (char c : name) {
            if (!std::isalnum(c) && c != ' ' && c != '-' && c != '_') {
                // 检查是否为中文字符
                if (static_cast<unsigned char>(c) < 128) {
                    return false;
                }
            }
        }
        return true;
    }

    // 密码哈希验证（SHA256格式）
    static bool validatePasswordHash(const std::string& hash) {
        std::regex pattern("^[a-fA-F0-9]{64}$");
        return std::regex_match(hash, pattern);
    }

    // 盐值验证（Base64格式，32字符）
    static bool validateSalt(const std::string& salt) {
        std::regex pattern("^[a-zA-Z0-9]{32}$");
        return std::regex_match(salt, pattern);
    }

    // 通知标题验证
    static bool validateNotificationTitle(const std::string& title) {
        return !title.empty() && title.length() <= 200;
    }

    // 通知内容验证
    static bool validateNotificationContent(const std::string& content) {
        return !content.empty() && content.length() <= 5000;
    }

    // 金额验证（正数，最多两位小数）
    static bool validateAmount(double amount) {
        return amount >= 0 && amount <= 999999.99;
    }

    // 分数验证（0-100）
    static bool validateScore(double score) {
        return score >= 0 && score <= 100;
    }

    // 课程名称验证
    static bool validateCourseName(const std::string& name) {
        return !name.empty() && name.length() <= 100;
    }

    // 课程代码验证
    static bool validateCourseCode(const std::string& code) {
        std::regex pattern("^[A-Z]{2,10}\\d{3,6}$");
        return std::regex_match(code, pattern);
    }

    // 活动标题验证
    static bool validateActivityTitle(const std::string& title) {
        return !title.empty() && title.length() <= 200;
    }

    // 活动描述验证
    static bool validateActivityDescription(const std::string& description) {
        return description.length() <= 1000;
    }

    // 地点验证
    static bool validateLocation(const std::string& location) {
        return location.length() <= 200;
    }

    // 文件路径验证
    static bool validateFilePath(const std::string& path) {
        if (path.empty() || path.length() > 500) return false;

        // 检查是否包含非法字符
        std::string invalidChars = "<>:/\\|?*";
        for (char c : path) {
            if (invalidChars.find(c) != std::string::npos) {
                return false;
            }
        }
        return true;
    }

    // 文件大小验证（字节）
    static bool validateFileSize(long long size) {
        return size >= 0 && size <= 100 * 1024 * 1024; // 最大100MB
    }

    // 文件类型验证
    static bool validateFileType(const std::string& type) {
        static const std::vector<std::string> allowedTypes = {
            "application/pdf",
            "application/msword",
            "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
            "application/vnd.ms-excel",
            "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
            "application/vnd.ms-powerpoint",
            "application/vnd.openxmlformats-officedocument.presentationml.presentation",
            "text/plain",
            "image/jpeg",
            "image/png",
            "image/gif"
        };

        return std::find(allowedTypes.begin(), allowedTypes.end(), type) != allowedTypes.end();
    }

    // 通用字符串清理（防止SQL注入等）
    static std::string sanitizeString(const std::string& input) {
        std::string output = input;

        // 移除或转义危险字符
        size_t pos = 0;
        while ((pos = output.find("\\", pos)) != std::string::npos) {
            output.replace(pos, 1, "\\\\");
            pos += 2;
        }

        pos = 0;
        while ((pos = output.find("'", pos)) != std::string::npos) {
            output.replace(pos, 1, "''");
            pos += 2;
        }

        pos = 0;
        while ((pos = output.find("\"", pos)) != std::string::npos) {
            output.replace(pos, 1, "\"\"");
            pos += 2;
        }

        return output;
    }

    // 整数范围验证
    static bool validateIntRange(int value, int min, int max) {
        return value >= min && value <= max;
    }

    // 浮点数范围验证
    static bool validateDoubleRange(double value, double min, double max) {
        return value >= min && value <= max;
    }

    // 日期格式验证 (YYYY-MM-DD)
    static bool validateDate(const std::string& date) {
        std::regex pattern("^\\d{4}-\\d{2}-\\d{2}$");
        if (!std::regex_match(date, pattern)) return false;

        // 进一步验证日期的有效性
        int year, month, day;
        char dash1, dash2;
        std::istringstream iss(date);
        iss >> year >> dash1 >> month >> dash2 >> day;

        if (dash1 != '-' || dash2 != '-') return false;
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;

        // 简单的月份天数验证
        static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int maxDays = daysInMonth[month - 1];

        // 闰年处理
        if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
            maxDays = 29;
        }

        return day <= maxDays;
    }

    // 时间格式验证 (HH:MM:SS)
    static bool validateTime(const std::string& time) {
        std::regex pattern("^\\d{2}:\\d{2}:\\d{2}$");
        if (!std::regex_match(time, pattern)) return false;

        int hour, minute, second;
        char colon1, colon2;
        std::istringstream iss(time);
        iss >> hour >> colon1 >> minute >> colon2 >> second;

        return colon1 == ':' && colon2 == ':' &&
               hour >= 0 && hour <= 23 &&
               minute >= 0 && minute <= 59 &&
               second >= 0 && second <= 59;
    }
};

#endif // VALIDATOR_H