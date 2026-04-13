#include "../../include/core/user/user_manager.h"
#include "../../include/database/database_connector.h"
#include "../../include/utils/logger.h"
#include <gtest/gtest.h>
#include <iostream>

class UserManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化日志
        Logger::getInstance().setLogLevel(LogLevel::DEBUG);
        Logger::getInstance().enableConsoleOutput(true);

        // 连接测试数据库
        auto& db = DatabaseConnector::getInstance();
        bool connected = db.connect("localhost", 3306, "test_user", "test_password", "test_db");
        if (!connected) {
            std::cerr << "Warning: Could not connect to test database, tests may fail" << std::endl;
        }

        userManager = std::make_unique<UserManager>();
    }

    void TearDown() override {
        userManager.reset();
        DatabaseConnector::getInstance().disconnect();
    }

    std::unique_ptr<UserManager> userManager;
};

TEST_F(UserManagerTest, UserCreation) {
    User testUser;
    testUser.username = "testuser";
    testUser.real_name = "Test User";
    testUser.email = "test@example.com";
    testUser.phone = "13800138000";
    testUser.class_id = 1;
    testUser.role = UserRole::STUDENT;
    testUser.salt = "testsalt12345678901234567890123456789012";
    testUser.password_hash = "testhash";

    // 测试用户创建
    auto result = userManager->createUser(testUser);
    EXPECT_TRUE(result.success) << "User creation should succeed";
}

TEST_F(UserManagerTest, UserValidation) {
    User invalidUser;
    invalidUser.username = "";  // 空用户名应该失败
    invalidUser.real_name = "Test User";
    invalidUser.class_id = 1;

    auto result = userManager->createUser(invalidUser);
    EXPECT_FALSE(result.success) << "Empty username should fail validation";
}

TEST_F(UserManagerTest, PasswordHashing) {
    std::string password = "testpassword123";
    std::string salt = "testsalt12345678901234567890123456789012";
    std::string hash1 = userManager->hashPassword(password, salt);
    std::string hash2 = userManager->hashPassword(password, salt);

    EXPECT_EQ(hash1, hash2) << "Same password and salt should produce same hash";

    std::string differentSalt = "differentsalt12345678901234567890123456789";
    std::string hash3 = userManager->hashPassword(password, differentSalt);
    EXPECT_NE(hash1, hash3) << "Different salt should produce different hash";
}

TEST_F(UserManagerTest, PasswordVerification) {
    std::string password = "testpassword123";
    std::string salt = userManager->generateSalt();
    std::string hash = userManager->hashPassword(password, salt);

    EXPECT_TRUE(userManager->verifyPassword(password, hash, salt))
        << "Correct password should verify successfully";

    EXPECT_FALSE(userManager->verifyPassword("wrongpassword", hash, salt))
        << "Wrong password should fail verification";
}

TEST_F(UserManagerTest, UserSearch) {
    // 这个测试需要数据库中有测试数据
    std::vector<User> users = userManager->searchUsers("test");
    EXPECT_NO_THROW(users) << "Search should not throw exceptions";
}

TEST_F(UserManagerTest, RoleBasedPermissions) {
    // 测试权限系统
    EXPECT_TRUE(userManager->hasPermission(1, "view_notifications"))
        << "Admin should have permission to view notifications";

    EXPECT_FALSE(userManager->hasPermission(999, "view_notifications"))
        << "Invalid user should not have permissions";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}