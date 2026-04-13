#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "../common.h"
#include "../core/user/user_manager.h"
#include "../core/notification/notification_manager.h"
#include <crow.h>
#include <memory>

class WebServer {
public:
    WebServer();
    ~WebServer();

    // 服务器配置
    bool initialize(const std::string& host, int port);
    void start();
    void stop();
    bool isRunning() const;

    // 路由配置
    void setupRoutes();
    void setupStaticRoutes();
    void setupApiRoutes();

private:
    // 服务器实例
    std::unique_ptr<crow::SimpleApp> app_;
    std::string host_;
    int port_;
    bool running_;

    // 业务管理器
    std::unique_ptr<UserManager> userManager_;
    std::unique_ptr<NotificationManager> notificationManager_;

    // 中间件
    void setupMiddlewares();

    // API处理函数
    // 用户相关API
    crow::response handleUserLogin(const crow::request& req);
    crow::response handleUserRegister(const crow::request& req);
    crow::response handleUserLogout(const crow::request& req);
    crow::response handleGetUserInfo(const crow::request& req, int user_id);
    crow::response handleUpdateUserInfo(const crow::request& req, int user_id);
    crow::response handleChangePassword(const crow::request& req, int user_id);
    crow::response handleGetUsers(const crow::request& req);

    // 通知相关API
    crow::response handlePublishNotification(const crow::request& req);
    crow::response handleUpdateNotification(const crow::request& req, int notification_id);
    crow::response handleDeleteNotification(const crow::request& req, int notification_id);
    crow::response handleGetNotifications(const crow::request& req);
    crow::response handleGetNotificationById(const crow::request& req, int notification_id);
    crow::response handleMarkNotificationRead(const crow::request& req, int notification_id);
    crow::response handleGetUnreadNotifications(const crow::request& req);
    crow::response handleGetUnreadCount(const crow::request& req);

    // 辅助函数
    std::string generateResponse(bool success, const std::string& message, const std::string& data = "");
    std::string generateErrorResponse(const std::string& message);
    std::string generateSuccessResponse(const std::string& message, const std::string& data = "");

    // 认证和权限检查
    bool authenticateRequest(const crow::request& req);
    bool checkPermission(const crow::request& req, const std::string& permission);
    std::optional<int> getUserIdFromToken(const std::string& token);

    // JSON解析辅助函数
    bool parseJsonRequest(const crow::request& req, crow::json::rvalue& json);
};

#endif // WEB_SERVER_H