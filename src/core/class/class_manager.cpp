#include "../../include/core/class/class_manager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/validator.h"
#include <sstream>

ClassManager::ClassManager() : db_(DatabaseConnector::getInstance()) {
    LOG_INFO("ClassManager initialized");
}

ClassManager::~ClassManager() {
    LOG_INFO("ClassManager destroyed");
}

Result ClassManager::createClass(const ClassInfo& classInfo) {
    if (!validateClassData(classInfo)) {
        return Result::Failure("班级数据验证失败");
    }

    return insertClass(classInfo);
}

Result ClassManager::updateClass(const ClassInfo& classInfo) {
    if (!validateClassData(classInfo)) {
        return Result::Failure("班级数据验证失败");
    }

    auto existingClass = getClassById(classInfo.id);
    if (!existingClass) {
        return Result::Failure("班级不存在");
    }

    return updateClassInDB(classInfo);
}

Result ClassManager::deleteClass(int class_id) {
    auto classInfo = getClassById(class_id);
    if (!classInfo) {
        return Result::Failure("班级不存在");
    }

    // 检查是否还有学生
    auto students = getUserManager().getUsersByClass(class_id);
    if (!students.empty()) {
        return Result::Failure("班级中还有学生，无法删除");
    }

    return deleteClassFromDB(class_id);
}

std::optional<ClassInfo> ClassManager::getClassById(int class_id) {
    std::stringstream query;
    query << "SELECT c.*, u1.real_name as monitor_name, u2.real_name as teacher_name "
          << "FROM classes c "
          << "LEFT JOIN users u1 ON c.monitor_id = u1.id "
          << "LEFT JOIN users u2 ON c.teacher_id = u2.id "
          << "WHERE c.id = " << class_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            ClassInfo classInfo;
            classInfo.id = dbResult.getInt("id");
            classInfo.name = dbResult.getString("name");
            classInfo.grade = dbResult.getString("grade");
            classInfo.major = dbResult.getString("major");
            classInfo.student_count = dbResult.getInt("student_count");
            classInfo.monitor_id = dbResult.getInt("monitor_id");
            classInfo.teacher_id = dbResult.getInt("teacher_id");
            classInfo.description = dbResult.getString("description");
            classInfo.is_active = dbResult.getBool("is_active");

            // 转换时间戳
            std::string createdAtStr = dbResult.getString("created_at");
            std::string updatedAtStr = dbResult.getString("updated_at");
            if (!createdAtStr.empty()) {
                classInfo.created_at = TimeUtils::parseTime(createdAtStr);
            }
            if (!updatedAtStr.empty()) {
                classInfo.updated_at = TimeUtils::parseTime(updatedAtStr);
            }

            return classInfo;
        }
    }

    return std::nullopt;
}

std::vector<ClassInfo> ClassManager::getAllClasses() {
    std::vector<ClassInfo> classes;

    std::string query = "SELECT * FROM classes WHERE is_active = TRUE ORDER BY grade, major, name";

    MYSQL_RES* result;
    if (db_.executeQuery(query, &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            ClassInfo classInfo;
            classInfo.id = dbResult.getInt("id");
            classInfo.name = dbResult.getString("name");
            classInfo.grade = dbResult.getString("grade");
            classInfo.major = dbResult.getString("major");
            classInfo.student_count = dbResult.getInt("student_count");
            classInfo.monitor_id = dbResult.getInt("monitor_id");
            classInfo.teacher_id = dbResult.getInt("teacher_id");
            classInfo.description = dbResult.getString("description");
            classInfo.is_active = dbResult.getBool("is_active");

            classes.push_back(classInfo);
        }
    }

    return classes;
}

std::vector<ClassInfo> ClassManager::getClassesByGrade(const std::string& grade) {
    std::vector<ClassInfo> classes;

    std::stringstream query;
    query << "SELECT * FROM classes WHERE grade = '" << grade << "' AND is_active = TRUE ORDER BY major, name";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            ClassInfo classInfo;
            classInfo.id = dbResult.getInt("id");
            classInfo.name = dbResult.getString("name");
            classInfo.grade = dbResult.getString("grade");
            classInfo.major = dbResult.getString("major");
            classInfo.student_count = dbResult.getInt("student_count");
            classInfo.monitor_id = dbResult.getInt("monitor_id");
            classInfo.teacher_id = dbResult.getInt("teacher_id");
            classInfo.description = dbResult.getString("description");
            classInfo.is_active = dbResult.getBool("is_active");

            classes.push_back(classInfo);
        }
    }

    return classes;
}

std::vector<ClassInfo> ClassManager::getClassesByMajor(const std::string& major) {
    std::vector<ClassInfo> classes;

    std::stringstream query;
    query << "SELECT * FROM classes WHERE major = '" << major << "' AND is_active = TRUE ORDER BY grade, name";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            ClassInfo classInfo;
            classInfo.id = dbResult.getInt("id");
            classInfo.name = dbResult.getString("name");
            classInfo.grade = dbResult.getString("grade");
            classInfo.major = dbResult.getString("major");
            classInfo.student_count = dbResult.getInt("student_count");
            classInfo.monitor_id = dbResult.getInt("monitor_id");
            classInfo.teacher_id = dbResult.getInt("teacher_id");
            classInfo.description = dbResult.getString("description");
            classInfo.is_active = dbResult.getBool("is_active");

            classes.push_back(classInfo);
        }
    }

    return classes;
}

Result ClassManager::assignMonitor(int class_id, int student_id) {
    // 验证学生是否属于该班级
    auto user = getUserManager().getUserById(student_id);
    if (!user || user->class_id != class_id) {
        return Result::Failure("学生不属于该班级");
    }

    // 验证学生角色
    if (user->role != UserRole::STUDENT && user->role != UserRole::CLASS_MONITOR) {
        return Result::Failure("只能指定学生或班长为班长");
    }

    std::stringstream query;
    query << "UPDATE classes SET monitor_id = " << student_id << " WHERE id = " << class_id;

    if (db_.execute(query.str())) {
        // 更新用户角色为班长
        user->role = UserRole::CLASS_MONITOR;
        auto updateResult = getUserManager().updateUser(*user);

        if (updateResult.success) {
            LOG_INFO_FMT("Monitor assigned: class_id=%d, student_id=%d", class_id, student_id);
            return Result::Success("班长指定成功");
        } else {
            // 回滚班级更新
            std::stringstream rollbackQuery;
            rollbackQuery << "UPDATE classes SET monitor_id = NULL WHERE id = " << class_id;
            db_.execute(rollbackQuery.str());
            return Result::Failure("班长指定失败: " + updateResult.message);
        }
    } else {
        return Result::Failure("班长指定失败: " + db_.getLastError());
    }
}

Result ClassManager::assignTeacher(int class_id, int teacher_id) {
    // 验证用户是否为教师
    auto user = getUserManager().getUserById(teacher_id);
    if (!user || (user->role != UserRole::TEACHER && user->role != UserRole::ADMIN)) {
        return Result::Failure("只能指定教师或管理员为辅导员");
    }

    std::stringstream query;
    query << "UPDATE classes SET teacher_id = " << teacher_id << " WHERE id = " << class_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Teacher assigned: class_id=%d, teacher_id=%d", class_id, teacher_id);
        return Result::Success("辅导员指定成功");
    } else {
        return Result::Failure("辅导员指定失败: " + db_.getLastError());
    }
}

Result ClassManager::updateStudentCount(int class_id) {
    std::stringstream query;
    query << "UPDATE classes c SET student_count = ("
          << "SELECT COUNT(*) FROM users u WHERE u.class_id = c.id AND u.is_active = TRUE"
          << ") WHERE c.id = " << class_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Student count updated for class_id=%d", class_id);
        return Result::Success("学生人数更新成功");
    } else {
        return Result::Failure("学生人数更新失败: " + db_.getLastError());
    }
}

std::vector<User> ClassManager::getClassMembers(int class_id) {
    return getUserManager().getUsersByClass(class_id);
}

ClassStatistics ClassManager::getClassStatistics(int class_id) {
    ClassStatistics stats;
    stats.class_id = class_id;

    // 获取班级基本信息
    auto classInfo = getClassById(class_id);
    if (classInfo) {
        stats.total_students = classInfo->student_count;
    }

    // 获取学生角色分布
    std::stringstream query;
    query << "SELECT role, COUNT(*) as count FROM users WHERE class_id = " << class_id
          << " AND is_active = TRUE GROUP BY role";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            int role = dbResult.getInt("role");
            int count = dbResult.getInt("count");

            switch (static_cast<UserRole>(role)) {
                case UserRole::STUDENT:
                    stats.regular_students = count;
                    break;
                case UserRole::CLASS_MONITOR:
                    stats.class_monitors = count;
                    break;
                case UserRole::STUDY_MONITOR:
                    stats.study_monitors = count;
                    break;
                case UserRole::LIFE_MONITOR:
                    stats.life_monitors = count;
                    break;
                case UserRole::SPORTS_MONITOR:
                    stats.sports_monitors = count;
                    break;
                default:
                    break;
            }
        }
    }

    return stats;
}

// 私有方法实现
bool ClassManager::validateClassData(const ClassInfo& classInfo) {
    if (classInfo.name.empty() || classInfo.grade.empty() || classInfo.major.empty()) {
        return false;
    }

    if (classInfo.name.length() > 100) {
        return false;
    }

    if (classInfo.grade.length() > 20) {
        return false;
    }

    if (classInfo.major.length() > 100) {
        return false;
    }

    if (classInfo.description.length() > 1000) {
        return false;
    }

    return true;
}

Result ClassManager::insertClass(const ClassInfo& classInfo) {
    std::stringstream query;
    query << "INSERT INTO classes (name, grade, major, description, monitor_id, teacher_id) VALUES ("
          << "'" << Validator::sanitizeString(classInfo.name) << "', "
          << "'" << Validator::sanitizeString(classInfo.grade) << "', "
          << "'" << Validator::sanitizeString(classInfo.major) << "', "
          << "'" << Validator::sanitizeString(classInfo.description) << "', "
          << (classInfo.monitor_id > 0 ? std::to_string(classInfo.monitor_id) : "NULL") << ", "
          << (classInfo.teacher_id > 0 ? std::to_string(classInfo.teacher_id) : "NULL")
          << ")";

    if (db_.execute(query.str())) {
        int class_id = mysql_insert_id(db_.getConnection());
        LOG_INFO_FMT("Class created successfully: ID %d, name: %s", class_id, classInfo.name.c_str());
        return Result::Success("班级创建成功");
    } else {
        LOG_ERROR_FMT("Failed to create class: %s", classInfo.name.c_str());
        return Result::Failure("班级创建失败: " + db_.getLastError());
    }
}

Result ClassManager::updateClassInDB(const ClassInfo& classInfo) {
    std::stringstream query;
    query << "UPDATE classes SET "
          << "name = '" << Validator::sanitizeString(classInfo.name) << "', "
          << "grade = '" << Validator::sanitizeString(classInfo.grade) << "', "
          << "major = '" << Validator::sanitizeString(classInfo.major) << "', "
          << "description = '" << Validator::sanitizeString(classInfo.description) << "', "
          << "monitor_id = " << (classInfo.monitor_id > 0 ? std::to_string(classInfo.monitor_id) : "NULL") << ", "
          << "teacher_id = " << (classInfo.teacher_id > 0 ? std::to_string(classInfo.teacher_id) : "NULL")
          << " WHERE id = " << classInfo.id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Class updated successfully: ID %d", classInfo.id);
        return Result::Success("班级更新成功");
    } else {
        LOG_ERROR_FMT("Failed to update class: ID %d", classInfo.id);
        return Result::Failure("班级更新失败: " + db_.getLastError());
    }
}

Result ClassManager::deleteClassFromDB(int class_id) {
    std::stringstream query;
    query << "UPDATE classes SET is_active = FALSE WHERE id = " << class_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Class deleted (soft delete): ID %d", class_id);
        return Result::Success("班级删除成功");
    } else {
        LOG_ERROR_FMT("Failed to delete class: ID %d", class_id);
        return Result::Failure("班级删除失败: " + db_.getLastError());
    }
}

UserManager& ClassManager::getUserManager() {
    static UserManager userManager;
    return userManager;
}