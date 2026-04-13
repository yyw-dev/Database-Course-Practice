#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "../common.h"
#include "../../database/database_connector.h"

// 用户信息结构
struct User {
    int id = 0;
    std::string username;
    std::string password_hash;
    std::string salt;
    std::string real_name;
    std::string email;
    std::string phone;
    UserRole role = UserRole::STUDENT;
    int class_id = 0;
    bool is_active = true;
    TimePoint created_at;
    TimePoint updated_at;

    // 转换为Map便于JSON序列化
    StringMap toMap() const;
    static User fromMap(const StringMap& map);
};

// 用户管理器类
class UserManager {
public:
    UserManager();
    ~UserManager();

    // 用户注册和登录
    Result registerUser(const User& user);
    Result login(const std::string& username, const std::string& password);
    Result logout(int user_id);

    // 用户信息管理
    Result createUser(const User& user);
    Result updateUser(const User& user);
    Result deleteUser(int user_id);
    std::optional<User> getUserById(int user_id);
    std::optional<User> getUserByUsername(const std::string& username);

    // 用户查询
    std::vector<User> getUsersByClass(int class_id);
    std::vector<User> getUsersByRole(UserRole role);
    std::vector<User> searchUsers(const std::string& keyword);

    // 密码管理
    Result changePassword(int user_id, const std::string& old_password, const std::string& new_password);
    Result resetPassword(int user_id, const std::string& new_password);

    // 权限检查
    bool hasPermission(int user_id, const std::string& permission);
    bool canAccessResource(int user_id, const std::string& resource, const std::string& action);

    // 用户状态管理
    Result activateUser(int user_id);
    Result deactivateUser(int user_id);

    // 批量操作
    Result batchCreateUsers(const std::vector<User>& users);
    Result batchUpdateUsers(const std::vector<User>& users);

private:
    DatabaseConnector& db_;

    // 内部辅助方法
    bool validateUserData(const User& user);
    std::string hashPassword(const std::string& password, const std::string& salt);
    bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt);
    std::string generateSalt();

    // 数据库操作
    Result insertUser(const User& user);
    Result updateUserInDB(const User& user);
    Result deleteUserFromDB(int user_id);
    std::optional<User> loadUserFromDB(int user_id);
};

#endif // USER_MANAGER_H