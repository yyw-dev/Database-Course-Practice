#include "../../include/core/study/study_manager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/validator.h"
#include <sstream>
#include <algorithm>
#include <cmath>

StudyManager::StudyManager() : db_(DatabaseConnector::getInstance()) {
    LOG_INFO("StudyManager initialized");
}

StudyManager::~StudyManager() {
    LOG_INFO("StudyManager destroyed");
}

// 课程管理实现
Result StudyManager::createCourse(const Course& course) {
    if (!validateCourseData(course)) {
        return Result::Failure("课程数据验证失败");
    }

    return insertCourse(course);
}

Result StudyManager::updateCourse(const Course& course) {
    if (!validateCourseData(course)) {
        return Result::Failure("课程数据验证失败");
    }

    auto existingCourse = getCourseById(course.id);
    if (!existingCourse) {
        return Result::Failure("课程不存在");
    }

    return updateCourseInDB(course);
}

Result StudyManager::deleteCourse(int course_id) {
    auto course = getCourseById(course_id);
    if (!course) {
        return Result::Failure("课程不存在");
    }

    return deleteCourseFromDB(course_id);
}

std::optional<Course> StudyManager::getCourseById(int course_id) {
    std::stringstream query;
    query << "SELECT * FROM courses WHERE id = " << course_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            Course course;
            course.id = dbResult.getInt("id");
            course.name = dbResult.getString("name");
            course.code = dbResult.getString("code");
            course.teacher_id = dbResult.getInt("teacher_id");
            course.class_id = dbResult.getInt("class_id");
            course.credit = dbResult.getDouble("credit");
            course.semester = dbResult.getString("semester");
            course.description = dbResult.getString("description");
            course.is_active = dbResult.getBool("is_active");

            return course;
        }
    }

    return std::nullopt;
}

std::vector<Course> StudyManager::getCoursesByClass(int class_id) {
    std::vector<Course> courses;

    std::stringstream query;
    query << "SELECT * FROM courses WHERE class_id = " << class_id << " AND is_active = TRUE ORDER BY name";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Course course;
            course.id = dbResult.getInt("id");
            course.name = dbResult.getString("name");
            course.code = dbResult.getString("code");
            course.teacher_id = dbResult.getInt("teacher_id");
            course.class_id = dbResult.getInt("class_id");
            course.credit = dbResult.getDouble("credit");
            course.semester = dbResult.getString("semester");
            course.description = dbResult.getString("description");
            course.is_active = dbResult.getBool("is_active");

            courses.push_back(course);
        }
    }

    return courses;
}

std::vector<Course> StudyManager::getCoursesByTeacher(int teacher_id) {
    std::vector<Course> courses;

    std::stringstream query;
    query << "SELECT * FROM courses WHERE teacher_id = " << teacher_id << " AND is_active = TRUE ORDER BY semester, name";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Course course;
            course.id = dbResult.getInt("id");
            course.name = dbResult.getString("name");
            course.code = dbResult.getString("code");
            course.teacher_id = dbResult.getInt("teacher_id");
            course.class_id = dbResult.getInt("class_id");
            course.credit = dbResult.getDouble("credit");
            course.semester = dbResult.getString("semester");
            course.description = dbResult.getString("description");
            course.is_active = dbResult.getBool("is_active");

            courses.push_back(course);
        }
    }

    return courses;
}

std::vector<Course> StudyManager::getCoursesBySemester(const std::string& semester) {
    std::vector<Course> courses;

    std::stringstream query;
    query << "SELECT * FROM courses WHERE semester = '" << semester << "' AND is_active = TRUE ORDER BY class_id, name";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Course course;
            course.id = dbResult.getInt("id");
            course.name = dbResult.getString("name");
            course.code = dbResult.getString("code");
            course.teacher_id = dbResult.getInt("teacher_id");
            course.class_id = dbResult.getInt("class_id");
            course.credit = dbResult.getDouble("credit");
            course.semester = dbResult.getString("semester");
            course.description = dbResult.getString("description");
            course.is_active = dbResult.getBool("is_active");

            courses.push_back(course);
        }
    }

    return courses;
}

// 课程安排管理实现
Result StudyManager::addCourseSchedule(const CourseSchedule& schedule) {
    if (schedule.course_id <= 0 || schedule.day_of_week < 1 || schedule.day_of_week > 7) {
        return Result::Failure("课程安排数据无效");
    }

    std::stringstream query;
    query << "INSERT INTO course_schedules (course_id, day_of_week, start_time, end_time, classroom, week_type) VALUES ("
          << schedule.course_id << ", "
          << schedule.day_of_week << ", "
          << "'" << schedule.start_time << "', "
          << "'" << schedule.end_time << "', "
          << "'" << Validator::sanitizeString(schedule.classroom) << "', "
          << "'" << schedule.week_type << "')";

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Course schedule added: course_id=%d, day=%d", schedule.course_id, schedule.day_of_week);
        return Result::Success("课程安排添加成功");
    } else {
        return Result::Failure("课程安排添加失败: " + db_.getLastError());
    }
}

Result StudyManager::updateCourseSchedule(const CourseSchedule& schedule) {
    if (schedule.id <= 0 || schedule.day_of_week < 1 || schedule.day_of_week > 7) {
        return Result::Failure("课程安排数据无效");
    }

    std::stringstream query;
    query << "UPDATE course_schedules SET "
          << "day_of_week = " << schedule.day_of_week << ", "
          << "start_time = '" << schedule.start_time << "', "
          << "end_time = '" << schedule.end_time << "', "
          << "classroom = '" << Validator::sanitizeString(schedule.classroom) << "', "
          << "week_type = '" << schedule.week_type << "' "
          << "WHERE id = " << schedule.id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Course schedule updated: id=%d", schedule.id);
        return Result::Success("课程安排更新成功");
    } else {
        return Result::Failure("课程安排更新失败: " + db_.getLastError());
    }
}

Result StudyManager::deleteCourseSchedule(int schedule_id) {
    std::stringstream query;
    query << "DELETE FROM course_schedules WHERE id = " << schedule_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Course schedule deleted: id=%d", schedule_id);
        return Result::Success("课程安排删除成功");
    } else {
        return Result::Failure("课程安排删除失败: " + db_.getLastError());
    }
}

std::vector<CourseSchedule> StudyManager::getCourseSchedules(int course_id) {
    std::vector<CourseSchedule> schedules;

    std::stringstream query;
    query << "SELECT * FROM course_schedules WHERE course_id = " << course_id << " ORDER BY day_of_week, start_time";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            CourseSchedule schedule;
            schedule.id = dbResult.getInt("id");
            schedule.course_id = dbResult.getInt("course_id");
            schedule.day_of_week = dbResult.getInt("day_of_week");
            schedule.start_time = dbResult.getString("start_time");
            schedule.end_time = dbResult.getString("end_time");
            schedule.classroom = dbResult.getString("classroom");
            schedule.week_type = dbResult.getString("week_type");

            schedules.push_back(schedule);
        }
    }

    return schedules;
}

std::vector<CourseSchedule> StudyManager::getClassSchedule(int class_id, const std::string& semester) {
    std::vector<CourseSchedule> schedules;

    std::stringstream query;
    query << "SELECT cs.* FROM course_schedules cs "
          << "JOIN courses c ON