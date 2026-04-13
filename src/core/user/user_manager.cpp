#include "../../include/core/user/user_manager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/validator.h"
#include <sstream>
#include <iomanip>

UserManager::UserManager() : db_(DatabaseConnector::getInstance()) {
    LOG_INFO("UserManager initialized");
}

UserManager::~UserManager() {
    LOG_INFO("UserManager destroyed");
}

Result UserManager::registerUser(const User& user) {
    // 验证用户数据
    if (!validateUserData(user)) {
        return Result::Failure("用户数据验证失败");
    }

    // 检查用户名是否已存在
    if (getUserByUsername(user.username)) {
        return Result::Failure("用户名已存在");
    }

    // 检查学号是否已存在（如果提供了学号）
    if (!user.student_id.empty()) {
        std::stringstream query;
        query << "SELECT id FROM users WHERE student_id = '" << user.student_id << "'";
        MYSQL_RES* result;
        if (db_.executeQuery(query.str(), &result)) {
            DatabaseResult dbResult(result);
            if (dbResult.next()) {
                return Result::Failure("学号已存在");
            }
        }
    }

    return createUser(user);
}

Result UserManager::login(const std::string& username, const std::string& password) {
    auto user = getUserByUsername(username);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    if (!user->is_active) {
        return Result::Failure("用户已被禁用");
    }

    if (!verifyPassword(password, user->password_hash, user->salt)) {
        return Result::Failure("密码错误");
    }

    // 更新最后登录时间
    std::stringstream updateQuery;
    updateQuery << "UPDATE users SET last_login_at = NOW() WHERE id = " << user->id;
    if (!db_.execute(updateQuery.str())) {
        LOG_ERROR("Failed to update last login time for user: " + username);
    }

    LOG_INFO_FMT("User %s logged in successfully", username.c_str());
    return Result::Success("登录成功");
}

Result UserManager::logout(int user_id) {
    auto user = getUserById(user_id);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    LOG_INFO_FMT("User %s logged out", user->username.c_str());
    return Result::Success("登出成功");
}

Result UserManager::createUser(const User& user) {
    if (!validateUserData(user)) {
        return Result::Failure("用户数据验证失败");
    }

    return insertUser(user);
}

Result UserManager::updateUser(const User& user) {
    if (!validateUserData(user)) {
        return Result::Failure("用户数据验证失败");
    }

    auto existingUser = getUserById(user.id);
    if (!existingUser) {
        return Result::Failure("用户不存在");
    }

    // 检查用户名是否被其他用户使用
    auto userByUsername = getUserByUsername(user.username);
    if (userByUsername && userByUsername->id != user.id) {
        return Result::Failure("用户名已被其他用户使用");
    }

    // 检查学号是否被其他用户使用
    if (!user.student_id.empty()) {
        std::stringstream query;
        query << "SELECT id FROM users WHERE student_id = '" << user.student_id
              << "' AND id != " << user.id;
        MYSQL_RES* result;
        if (db_.executeQuery(query.str(), &result)) {
            DatabaseResult dbResult(result);
            if (dbResult.next()) {
                return Result::Failure("学号已被其他用户使用");
            }
        }
    }

    return updateUserInDB(user);
}

Result UserManager::deleteUser(int user_id) {
    auto user = getUserById(user_id);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    return deleteUserFromDB(user_id);
}

std::optional<User> UserManager::getUserById(int user_id) {
    std::stringstream query;
    query << "SELECT u.*, c.name as class_name FROM users u "
          << "LEFT JOIN classes c ON u.class_id = c.id "
          << "WHERE u.id = " << user_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            User user;
            user.id = dbResult.getInt("id");
            user.username = dbResult.getString("username");
            user.password_hash = dbResult.getString("password_hash");
            user.salt = dbResult.getString("salt");
            user.real_name = dbResult.getString("real_name");
            user.email = dbResult.getString("email");
            user.phone = dbResult.getString("phone");

            std::string roleStr = dbResult.getString("role");
            if (roleStr == "student") user.role = UserRole::STUDENT;
            else if (roleStr == "class_monitor") user.role = UserRole::CLASS_MONITOR;
            else if (roleStr == "study_monitor") user.role = UserRole::STUDY_MONITOR;
            else if (roleStr == "life_monitor") user.role = UserRole::LIFE_MONITOR;
            else if (roleStr == "sports_monitor") user.role = UserRole::SPORTS_MONITOR;
            else if (roleStr == "teacher") user.role = UserRole::TEACHER;
            else if (roleStr == "admin") user.role = UserRole::ADMIN;
            else user.role = UserRole::STUDENT;

            user.class_id = dbResult.getInt("class_id");
            user.student_id = dbResult.getString("student_id");
            user.is_active = dbResult.getBool("is_active");

            // 转换时间戳
            std::string createdAtStr = dbResult.getString("created_at");
            std::string updatedAtStr = dbResult.getString("updated_at");
            if (!createdAtStr.empty()) {
                user.created_at = TimeUtils::parseTime(createdAtStr);
            }
            if (!updatedAtStr.empty()) {
                user.updated_at = TimeUtils::parseTime(updatedAtStr);
            }

            return user;
        }
    }

    return std::nullopt;
}

std::optional<User> UserManager::getUserByUsername(const std::string& username) {
    std::stringstream query;
    query << "SELECT u.*, c.name as class_name FROM users u "
          << "LEFT JOIN classes c ON u.class_id = c.id "
          << "WHERE u.username = '" << username << "'";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            User user;
            user.id = dbResult.getInt("id");
            user.username = dbResult.getString("username");
            user.password_hash = dbResult.getString("password_hash");
            user.salt = dbResult.getString("salt");
            user.real_name = dbResult.getString("real_name");
            user.email = dbResult.getString("email");
            user.phone = dbResult.getString("phone");

            std::string roleStr = dbResult.getString("role");
            if (roleStr == "student") user.role = UserRole::STUDENT;
            else if (roleStr == "class_monitor") user.role = UserRole::CLASS_MONITOR;
            else if (roleStr == "study_monitor") user.role = UserRole::STUDY_MONITOR;
            else if (roleStr == "life_monitor") user.role = UserRole::LIFE_MONITOR;
            else if (roleStr == "sports_monitor") user.role = UserRole::SPORTS_MONITOR;
            else if (roleStr == "teacher") user.role = UserRole::TEACHER;
            else if (roleStr == "admin") user.role = UserRole::ADMIN;
            else user.role = UserRole::STUDENT;

            user.class_id = dbResult.getInt("class_id");
            user.student_id = dbResult.getString("student_id");
            user.is_active = dbResult.getBool("is_active");

            return user;
        }
    }

    return std::nullopt;
}

std::vector<User> UserManager::getUsersByClass(int class_id) {
    std::vector<User> users;

    std::stringstream query;
    query << "SELECT * FROM users WHERE class_id = " << class_id << " ORDER BY student_id";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            User user;
            user.id = dbResult.getInt("id");
            user.username = dbResult.getString("username");
            user.real_name = dbResult.getString("real_name");
            user.email = dbResult.getString("email");
            user.phone = dbResult.getString("phone");

            std::string roleStr = dbResult.getString("role");
            if (roleStr == "student") user.role = UserRole::STUDENT;
            else if (roleStr == "class_monitor") user.role = UserRole::CLASS_MONITOR;
            else if (roleStr == "study_monitor") user.role = UserRole::STUDY_MONITOR;
            else if (roleStr == "life_monitor") user.role = UserRole::LIFE_MONITOR;
            else if (roleStr == "sports_monitor") user.role = UserRole::SPORTS_MONITOR;
            else if (roleStr == "teacher") user.role = UserRole::TEACHER;
            else if (roleStr == "admin") user.role = UserRole::ADMIN;
            else user.role = UserRole::STUDENT;

            user.class_id = dbResult.getInt("class_id");
            user.student_id = dbResult.getString("student_id");
            user.is_active = dbResult.getBool("is_active");

            users.push_back(user);
        }
    }

    return users;
}

std::vector<User> UserManager::getUsersByRole(UserRole role) {
    std::vector<User> users;

    std::string roleStr;
    switch (role) {
        case UserRole::STUDENT: roleStr = "student"; break;
        case UserRole::CLASS_MONITOR: roleStr = "class_monitor"; break;
        case UserRole::STUDY_MONITOR: roleStr = "study_monitor"; break;
        case UserRole::LIFE_MONITOR: roleStr = "life_monitor"; break;
        case UserRole::SPORTS_MONITOR: roleStr = "sports_monitor"; break;
        case UserRole::TEACHER: roleStr = "teacher"; break;
        case UserRole::ADMIN: roleStr = "admin"; break;
    }

    std::stringstream query;
    query << "SELECT * FROM users WHERE role = '" << roleStr << "' ORDER BY class_id, student_id";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            User user;
            user.id = dbResult.getInt("id");
            user.username = dbResult.getString("username");
            user.real_name = dbResult.getString("real_name");
            user.email = dbResult.getString("email");
            user.phone = dbResult.getString("phone");
            user.role = role;
            user.class_id = dbResult.getInt("class_id");
            user.student_id = dbResult.getString("student_id");
            user.is_active = dbResult.getBool("is_active");

            users.push_back(user);
        }
    }

    return users;
}

std::vector<User> UserManager::searchUsers(const std::string& keyword) {
    std::vector<User> users;

    std::stringstream query;
    query << "SELECT * FROM users WHERE "
          << "real_name LIKE '%" << keyword << "%' OR "
          << "username LIKE '%" << keyword << "%' OR "
          << "student_id LIKE '%" << keyword << "%' "
          << "ORDER BY class_id, student_id";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            User user;
            user.id = dbResult.getInt("id");
            user.username = dbResult.getString("username");
            user.real_name = dbResult.getString("real_name");
            user.email = dbResult.getString("email");
            user.phone = dbResult.getString("phone");

            std::string roleStr = dbResult.getString("role");
            if (roleStr == "student") user.role = UserRole::STUDENT;
            else if (roleStr == "class_monitor") user.role = UserRole::CLASS_MONITOR;
            else if (roleStr == "study_monitor") user.role = UserRole::STUDY_MONITOR;
            else if (roleStr == "life_monitor") user.role = UserRole::LIFE_MONITOR;
            else if (roleStr == "sports_monitor") user.role = UserRole::SPORTS_MONITOR;
            else if (roleStr == "teacher") user.role = UserRole::TEACHER;
            else if (roleStr == "admin") user.role = UserRole::ADMIN;
            else user.role = UserRole::STUDENT;

            user.class_id = dbResult.getInt("class_id");
            user.student_id = dbResult.getString("student_id");
            user.is_active = dbResult.getBool("is_active");

            users.push_back(user);
        }
    }

    return users;
}

Result UserManager::changePassword(int user_id, const std::string& old_password, const std::string& new_password) {
    auto user = getUserById(user_id);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    if (!verifyPassword(old_password, user->password_hash, user->salt)) {
        return Result::Failure("原密码错误");
    }

    if (!Validator::validatePassword(new_password)) {
        return Result::Failure("新密码不符合安全要求");
    }

    std::string new_salt = generateSalt();
    std::string new_hash = hashPassword(new_password, new_salt);

    std::stringstream query;
    query << "UPDATE users SET password_hash = '" << new_hash
          << "', salt = '" << new_salt
          << "' WHERE id = " << user_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Password changed successfully for user ID: %d", user_id);
        return Result::Success("密码修改成功");
    } else {
        LOG_ERROR_FMT("Failed to change password for user ID: %d", user_id);
        return Result::Failure("密码修改失败");
    }
}

Result UserManager::resetPassword(int user_id, const std::string& new_password) {
    auto user = getUserById(user_id);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    if (!Validator::validatePassword(new_password)) {
        return Result::Failure("新密码不符合安全要求");
    }

    std::string new_salt = generateSalt();
    std::string new_hash = hashPassword(new_password, new_salt);

    std::stringstream query;
    query << "UPDATE users SET password_hash = '" << new_hash
          << "', salt = '" << new_salt
          << "' WHERE id = " << user_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Password reset successfully for user ID: %d", user_id);
        return Result::Success("密码重置成功");
    } else {
        LOG_ERROR_FMT("Failed to reset password for user ID: %d", user_id);
        return Result::Failure("密码重置失败");
    }
}

bool UserManager::hasPermission(int user_id, const std::string& permission) {
    auto user = getUserById(user_id);
    if (!user || !user->is_active) {
        return false;
    }

    // 管理员拥有所有权限
    if (user->role == UserRole::ADMIN) {
        return true;
    }

    // 根据角色判断权限（这里可以根据实际需求扩展）
    switch (user->role) {
        case UserRole::TEACHER:
            return permission == "manage_grades" || permission == "view_students" ||
                   permission == "publish_notifications" || permission == "manage_attendance";
        case UserRole::CLASS_MONITOR:
            return permission == "manage_activities" || permission == "view_class_info" ||
                   permission == "publish_notifications";
        case UserRole::STUDY_MONITOR:
            return permission == "manage_study_materials" || permission == "view_grades";
        case UserRole::LIFE_MONITOR:
            return permission == "manage_finance" || permission == "view_class_info";
        case UserRole::SPORTS_MONITOR:
            return permission == "manage_activities" || permission == "view_class_info";
        case UserRole::STUDENT:
            return permission == "view_notifications" || permission == "apply_leave" ||
                   permission == "register_activity" || permission == "view_own_grades";
        default:
            return false;
    }
}

bool UserManager::canAccessResource(int user_id, const std::string& resource, const std::string& action) {
    auto user = getUserById(user_id);
    if (!user || !user->is_active) {
        return false;
    }

    // 管理员可以访问所有资源
    if (user->role == UserRole::ADMIN) {
        return true;
    }

    // 基于资源的访问控制逻辑（可以根据实际需求扩展）
    if (resource == "grades") {
        if (action == "view") {
            return user->role == UserRole::TEACHER || user->role == UserRole::STUDY_MONITOR;
        } else if (action == "edit") {
            return user->role == UserRole::TEACHER;
        }
    } else if (resource == "finance") {
        if (action == "view") {
            return user->role == UserRole::LIFE_MONITOR || user->role == UserRole::CLASS_MONITOR;
        } else if (action == "edit") {
            return user->role == UserRole::LIFE_MONITOR;
        }
    }

    return false;
}

Result UserManager::activateUser(int user_id) {
    auto user = getUserById(user_id);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    std::stringstream query;
    query << "UPDATE users SET is_active = TRUE WHERE id = " << user_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("User activated: ID %d", user_id);
        return Result::Success("用户激活成功");
    } else {
        LOG_ERROR_FMT("Failed to activate user: ID %d", user_id);
        return Result::Failure("用户激活失败");
    }
}

Result UserManager::deactivateUser(int user_id) {
    auto user = getUserById(user_id);
    if (!user) {
        return Result::Failure("用户不存在");
    }

    std::stringstream query;
    query << "UPDATE users SET is_active = FALSE WHERE id = " << user_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("User deactivated: ID %d", user_id);
        return Result::Success("用户禁用成功");
    } else {
        LOG_ERROR_FMT("Failed to deactivate user: ID %d", user_id);
        return Result::Failure("用户禁用失败");
    }
}

Result UserManager::batchCreateUsers(const std::vector<User>& users) {
    if (users.empty()) {
        return Result::Failure("用户列表为空");
    }

    if (!db_.beginTransaction()) {
        return Result::Failure("开始事务失败");
    }

    bool success = true;
    for (const auto& user : users) {
        if (!validateUserData(user)) {
            success = false;
            break;
        }

        if (!insertUser(user).success) {
            success = false;
            break;
        }
    }

    if (success) {
        if (db_.commitTransaction()) {
            LOG_INFO_FMT("Batch created %zu users successfully", users.size());
            return Result::Success("批量创建用户成功");
        } else {
            db_.rollbackTransaction();
            return Result::Failure("提交事务失败");
        }
    } else {
        db_.rollbackTransaction();
        return Result::Failure("批量创建用户失败");
    }
}

Result UserManager::batchUpdateUsers(const std::vector<User>& users) {
    if (users.empty()) {
        return Result::Failure("用户列表为空");
    }

    if (!db_.beginTransaction()) {
        return Result::Failure("开始事务失败");
    }

    bool success = true;
    for (const auto& user : users) {
        if (!validateUserData(user)) {
            success = false;
            break;
        }

        if (!updateUserInDB(user).success) {
            success = false;
            break;
        }
    }

    if (success) {
        if (db_.commitTransaction()) {
            LOG_INFO_FMT("Batch updated %zu users successfully", users.size());
            return Result::Success("批量更新用户成功");
        } else {
            db_.rollbackTransaction();
            return Result::Failure("提交事务失败");
        }
    } else {
        db_.rollbackTransaction();
        return Result::Failure("批量更新用户失败");
    }
}

// 私有方法实现
bool UserManager::validateUserData(const User& user) {
    if (user.username.empty() || user.real_name.empty()) {
        return false;
    }

    if (!Validator::validateUsername(user.username)) {
        return false;
    }

    if (!user.password_hash.empty() && !Validator::validatePasswordHash(user.password_hash)) {
        return false;
    }

    if (!user.email.empty() && !Validator::validateEmail(user.email)) {
        return false;
    }

    if (!user.phone.empty() && !Validator::validatePhone(user.phone)) {
        return false;
    }

    if (user.student_id.empty() && user.role != UserRole::TEACHER && user.role != UserRole::ADMIN) {
        return false;
    }

    return true;
}

std::string UserManager::hashPassword(const std::string& password, const std::string& salt) {
    return CryptoUtils::sha256(password + salt);
}

bool UserManager::verifyPassword(const std::string& password, const std::string& hash, const std::string& salt) {
    return CryptoUtils::verifyPassword(password, hash, salt);
}

std::string UserManager::generateSalt() {
    return CryptoUtils::generateSalt();
}

Result UserManager::insertUser(const User& user) {
    std::string password_hash = user.password_hash;
    std::string salt = user.salt;

    // 如果密码哈希为空，说明是新用户，需要生成密码哈希
    if (password_hash.empty()) {
        // 这里应该有一个默认密码或者要求提供密码
        return Result::Failure("需要提供密码");
    }

    std::stringstream query;
    query << "INSERT INTO users (username, password_hash, salt, real_name, email, phone, "
          << "role, class_id, student_id, is_active) VALUES ("
          << "'" << user.username << "', "
          << "'" << password_hash << "', "
          << "'" << salt << "', "
          << "'" << user.real_name << "', "
          << "'" << user.email << "', "
          << "'" << user.phone << "', "
          << "'";

    switch (user.role) {
        case UserRole::STUDENT: query << "student"; break;
        case UserRole::CLASS_MONITOR: query << "class_monitor"; break;
        case UserRole::STUDY_MONITOR: query << "study_monitor"; break;
        case UserRole::LIFE_MONITOR: query << "life_monitor"; break;
        case UserRole::SPORTS_MONITOR: query << "sports_monitor"; break;
        case UserRole::TEACHER: query << "teacher"; break;
        case UserRole::ADMIN: query << "admin"; break;
        default: query << "student"; break;
    }

    query << "', "
          << user.class_id << ", "
          << "'" << user.student_id << "', "
          << (user.is_active ? "TRUE" : "FALSE")
          << ")";

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("User created successfully: %s", user.username.c_str());
        return Result::Success("用户创建成功");
    } else {
        LOG_ERROR_FMT("Failed to create user: %s", user.username.c_str());
        return Result::Failure("用户创建失败: " + db_.getLastError());
    }
}

Result UserManager::updateUserInDB(const User& user) {
    std::stringstream query;
    query << "UPDATE users SET "
          << "username = '" << user.username << "', "
          << "real_name = '" << user.real_name << "', "
          << "email = '" << user.email << "', "
          << "phone = '" << user.phone << "', "
          << "role = '";

    switch (user.role) {
        case UserRole::STUDENT: query << "student"; break;
        case UserRole::CLASS_MONITOR: query << "class_monitor"; break;
        case UserRole::STUDY_MONITOR: query << "study_monitor"; break;
        case UserRole::LIFE_MONITOR: query << "life_monitor"; break;
        case UserRole::SPORTS_MONITOR: query << "sports_monitor"; break;
        case UserRole::TEACHER: query << "teacher"; break;
        case UserRole::ADMIN: query << "admin"; break;
        default: query << "student"; break;
    }

    query << "', "
          << "class_id = " << user.class_id << ", "
          << "student_id = '" << user.student_id << "', "
          << "is_active = " << (user.is_active ? "TRUE" : "FALSE")
          << " WHERE id = " << user.id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("User updated successfully: %s", user.username.c_str());
        return Result::Success("用户更新成功");
    } else {
        LOG_ERROR_FMT("Failed to update user: %s", user.username.c_str());
        return Result::Failure("用户更新失败: " + db_.getLastError());
    }
}

Result UserManager::deleteUserFromDB(int user_id) {
    std::stringstream query;
    query << "DELETE FROM users WHERE id = " << user_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("User deleted successfully: ID %d", user_id);
        return Result::Success("用户删除成功");
    } else {
        LOG_ERROR_FMT("Failed to delete user: ID %d", user_id);
        return Result::Failure("用户删除失败: " + db_.getLastError());
    }
}

std::optional<User> UserManager::loadUserFromDB(int user_id) {
    return getUserById(user_id);
}