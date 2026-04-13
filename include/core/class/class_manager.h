#ifndef CLASS_MANAGER_H
#define CLASS_MANAGER_H

#include "../common.h"
#include "../../database/database_connector.h"
#include "../user/user_manager.h"

// 班级信息结构
struct ClassInfo {
    int id = 0;
    std::string name;
    std::string grade;
    std::string major;
    int student_count = 0;
    int monitor_id = 0;        // 班长ID
    int teacher_id = 0;        // 辅导员ID
    std::string description;
    bool is_active = true;
    TimePoint created_at;
    TimePoint updated_at;

    // 转换为Map便于JSON序列化
    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["name"] = name;
        map["grade"] = grade;
        map["major"] = major;
        map["student_count"] = std::to_string(student_count);
        map["monitor_id"] = std::to_string(monitor_id);
        map["teacher_id"] = std::to_string(teacher_id);
        map["description"] = description;
        map["is_active"] = is_active ? "true" : "false";
        return map;
    }

    static ClassInfo fromMap(const StringMap& map) {
        ClassInfo classInfo;
        if (map.count("id")) classInfo.id = std::stoi(map.at("id"));
        if (map.count("name")) classInfo.name = map.at("name");
        if (map.count("grade")) classInfo.grade = map.at("grade");
        if (map.count("major")) classInfo.major = map.at("major");
        if (map.count("student_count")) classInfo.student_count = std::stoi(map.at("student_count"));
        if (map.count("monitor_id")) classInfo.monitor_id = std::stoi(map.at("monitor_id"));
        if (map.count("teacher_id")) classInfo.teacher_id = std::stoi(map.at("teacher_id"));
        if (map.count("description")) classInfo.description = map.at("description");
        if (map.count("is_active")) classInfo.is_active = map.at("is_active") == "true";
        return classInfo;
    }
};

// 班级统计信息结构
struct ClassStatistics {
    int class_id = 0;
    int total_students = 0;
    int regular_students = 0;
    int class_monitors = 0;
    int study_monitors = 0;
    int life_monitors = 0;
    int sports_monitors = 0;
    int teachers = 0;

    // 转换为Map便于JSON序列化
    StringMap toMap() const {
        StringMap map;
        map["class_id"] = std::to_string(class_id);
        map["total_students"] = std::to_string(total_students);
        map["regular_students"] = std::to_string(regular_students);
        map["class_monitors"] = std::to_string(class_monitors);
        map["study_monitors"] = std::to_string(study_monitors);
        map["life_monitors"] = std::to_string(life_monitors);
        map["sports_monitors"] = std::to_string(sports_monitors);
        map["teachers"] = std::to_string(teachers);
        return map;
    }
};

// 班级管理器类
class ClassManager {
public:
    ClassManager();
    ~ClassManager();

    // 班级CRUD操作
    Result createClass(const ClassInfo& classInfo);
    Result updateClass(const ClassInfo& classInfo);
    Result deleteClass(int class_id);
    std::optional<ClassInfo> getClassById(int class_id);

    // 班级查询
    std::vector<ClassInfo> getAllClasses();
    std::vector<ClassInfo> getClassesByGrade(const std::string& grade);
    std::vector<ClassInfo> getClassesByMajor(const std::string& major);

    // 班委管理
    Result assignMonitor(int class_id, int student_id);
    Result assignTeacher(int class_id, int teacher_id);

    // 班级成员管理
    std::vector<User> getClassMembers(int class_id);
    Result updateStudentCount(int class_id);

    // 班级统计
    ClassStatistics getClassStatistics(int class_id);

private:
    DatabaseConnector& db_;

    // 内部辅助方法
    bool validateClassData(const ClassInfo& classInfo);
    UserManager& getUserManager();

    // 数据库操作
    Result insertClass(const ClassInfo& classInfo);
    Result updateClassInDB(const ClassInfo& classInfo);
    Result deleteClassFromDB(int class_id);
};

#endif // CLASS_MANAGER_H