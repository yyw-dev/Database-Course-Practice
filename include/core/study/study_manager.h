#ifndef STUDY_MANAGER_H
#define STUDY_MANAGER_H

#include "../common.h"
#include "../../database/database_connector.h"

// 课程信息结构
struct Course {
    int id = 0;
    std::string name;
    std::string code;
    int teacher_id = 0;
    int class_id = 0;
    double credit = 0.0;
    std::string semester;
    std::string description;
    bool is_active = true;
    TimePoint created_at;
    TimePoint updated_at;

    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["name"] = name;
        map["code"] = code;
        map["teacher_id"] = std::to_string(teacher_id);
        map["class_id"] = std::to_string(class_id);
        map["credit"] = std::to_string(credit);
        map["semester"] = semester;
        map["description"] = description;
        map["is_active"] = is_active ? "true" : "false";
        return map;
    }

    static Course fromMap(const StringMap& map) {
        Course course;
        if (map.count("id")) course.id = std::stoi(map.at("id"));
        if (map.count("name")) course.name = map.at("name");
        if (map.count("code")) course.code = map.at("code");
        if (map.count("teacher_id")) course.teacher_id = std::stoi(map.at("teacher_id"));
        if (map.count("class_id")) course.class_id = std::stoi(map.at("class_id"));
        if (map.count("credit")) course.credit = std::stod(map.at("credit"));
        if (map.count("semester")) course.semester = map.at("semester");
        if (map.count("description")) course.description = map.at("description");
        if (map.count("is_active")) course.is_active = map.at("is_active") == "true";
        return course;
    }
};

// 课程安排结构
struct CourseSchedule {
    int id = 0;
    int course_id = 0;
    int day_of_week = 0;  // 1-7, 周一到周日
    std::string start_time;
    std::string end_time;
    std::string classroom;
    std::string week_type; // all, odd, even

    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["course_id"] = std::to_string(course_id);
        map["day_of_week"] = std::to_string(day_of_week);
        map["start_time"] = start_time;
        map["end_time"] = end_time;
        map["classroom"] = classroom;
        map["week_type"] = week_type;
        return map;
    }
};

// 成绩信息结构
struct Grade {
    int id = 0;
    int student_id = 0;
    int course_id = 0;
    std::string grade_type;  // regular, midterm, final, total
    double score = 0.0;
    double grade_point = 0.0;
    std::string remarks;
    int recorded_by = 0;
    TimePoint recorded_at;

    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["student_id"] = std::to_string(student_id);
        map["course_id"] = std::to_string(course_id);
        map["grade_type"] = grade_type;
        map["score"] = std::to_string(score);
        map["grade_point"] = std::to_string(grade_point);
        map["remarks"] = remarks;
        map["recorded_by"] = std::to_string(recorded_by);
        return map;
    }
};

// 学习资料结构
struct StudyMaterial {
    int id = 0;
    std::string title;
    std::string description;
    std::string file_path;
    long long file_size = 0;
    std::string file_type;
    int uploader_id = 0;
    int course_id = 0;
    int download_count = 0;
    TimePoint created_at;

    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["title"] = title;
        map["description"] = description;
        map["file_path"] = file_path;
        map["file_size"] = std::to_string(file_size);
        map["file_type"] = file_type;
        map["uploader_id"] = std::to_string(uploader_id);
        map["course_id"] = std::to_string(course_id);
        map["download_count"] = std::to_string(download_count);
        return map;
    }
};

// 考勤记录结构
struct AttendanceRecord {
    int id = 0;
    int student_id = 0;
    int course_id = 0;
    std::string date;
    std::string status;  // present, absent, late, sick_leave, personal_leave
    std::string remarks;
    int recorded_by = 0;
    TimePoint recorded_at;

    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["student_id"] = std::to_string(student_id);
        map["course_id"] = std::to_string(course_id);
        map["date"] = date;
        map["status"] = status;
        map["remarks"] = remarks;
        map["recorded_by"] = std::to_string(recorded_by);
        return map;
    }
};

// 学习管理器类
class StudyManager {
public:
    StudyManager();
    ~StudyManager();

    // 课程管理
    Result createCourse(const Course& course);
    Result updateCourse(const Course& course);
    Result deleteCourse(int course_id);
    std::optional<Course> getCourseById(int course_id);
    std::vector<Course> getCoursesByClass(int class_id);
    std::vector<Course> getCoursesByTeacher(int teacher_id);
    std::vector<Course> getCoursesBySemester(const std::string& semester);

    // 课程安排管理
    Result addCourseSchedule(const CourseSchedule& schedule);
    Result updateCourseSchedule(const CourseSchedule& schedule);
    Result deleteCourseSchedule(int schedule_id);
    std::vector<CourseSchedule> getCourseSchedules(int course_id);
    std::vector<CourseSchedule> getClassSchedule(int class_id, const std::string& semester);

    // 成绩管理
    Result recordGrade(const Grade& grade);
    Result updateGrade(const Grade& grade);
    Result deleteGrade(int grade_id);
    std::optional<Grade> getGradeById(int grade_id);
    std::vector<Grade> getGradesByStudent(int student_id);
    std::vector<Grade> getGradesByCourse(int course_id);
    std::optional<Grade> getStudentCourseGrade(int student_id, int course_id, const std::string& grade_type);

    // 成绩统计
    double calculateStudentGPA(int student_id);
    double calculateClassAverage(int course_id);
    std::map<std::string, double> getGradeDistribution(int course_id);
    std::vector<Grade> getTopStudents(int course_id, int limit = 10);

    // 学习资料管理
    Result uploadMaterial(const StudyMaterial& material);
    Result deleteMaterial(int material_id);
    std::optional<StudyMaterial> getMaterialById(int material_id);
    std::vector<StudyMaterial> getMaterialsByCourse(int course_id);
    Result incrementDownloadCount(int material_id);

    // 考勤管理
    Result recordAttendance(const AttendanceRecord& record);
    Result updateAttendance(const AttendanceRecord& record);
    std::vector<AttendanceRecord> getAttendanceByStudent(int student_id, int course_id);
    std::vector<AttendanceRecord> getAttendanceByCourse(int course_id, const std::string& date);
    double calculateAttendanceRate(int student_id, int course_id);

private:
    DatabaseConnector& db_;

    // 内部辅助方法
    bool validateCourseData(const Course& course);
    bool validateGradeData(const Grade& grade);
    bool validateMaterialData(const StudyMaterial& material);
    bool validateAttendanceData(const AttendanceRecord& record);

    // 数据库操作
    Result insertCourse(const Course& course);
    Result updateCourseInDB(const Course& course);
    Result deleteCourseFromDB(int course_id);

    Result insertGrade(const Grade& grade);
    Result updateGradeInDB(const Grade& grade);

    Result insertMaterial(const StudyMaterial& material);

    Result insertAttendance(const AttendanceRecord& record);
    Result updateAttendanceInDB(const AttendanceRecord& record);

    // 成绩转换
    double scoreToGradePoint(double score);
};

#endif // STUDY_MANAGER_H