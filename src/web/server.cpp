#include "../../include/web/server.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/config_loader.h"
#include <thread>
#include <chrono>
#include <iostream>

WebServer::WebServer() : port_(8080), running_(false) {
    app_ = std::make_unique<crow::SimpleApp>();
    userManager_ = std::make_unique<UserManager>();
    notificationManager_ = std::make_unique<NotificationManager>();

    LOG_INFO("WebServer initialized");
}

WebServer::~WebServer() {
    stop();
    LOG_INFO("WebServer destroyed");
}

bool WebServer::initialize(const std::string& host, int port) {
    host_ = host;
    port_ = port;

    try {
        setupMiddlewares();
        setupRoutes();
        LOG_INFO_FMT("WebServer initialized with host: %s, port: %d", host.c_str(), port);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Failed to initialize WebServer: %s", e.what());
        return false;
    }
}

void WebServer::start() {
    if (running_) {
        LOG_WARNING("WebServer is already running");
        return;
    }

    try {
        LOG_INFO_FMT("Starting WebServer on %s:%d", host_.c_str(), port_);

        // 启动服务器
        app_->bindaddr(host_).port(port_).multithreaded().run();
        running_ = true;

        LOG_INFO("WebServer started successfully");
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Failed to start WebServer: %s", e.what());
        running_ = false;
    }
}

void WebServer::stop() {
    if (!running_) {
        return;
    }

    try {
        app_->stop();
        running_ = false;
        LOG_INFO("WebServer stopped");
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Error stopping WebServer: %s", e.what());
    }
}

bool WebServer::isRunning() const {
    return running_;
}

void WebServer::setupMiddlewares() {
    // CORS中间件
    CROW_ROUTE((*app_), "/api/cors")
    .methods("OPTIONS"_method)
    ([](const crow::request& req) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.code = 200;
        return res;
    });

    // 请求日志中间件
    app_->loglevel(crow::LogLevel::INFO);
}

void WebServer::setupRoutes() {
    setupStaticRoutes();
    setupApiRoutes();
}

void WebServer::setupStaticRoutes() {
    // 静态文件服务
    CROW_ROUTE((*app_), "/")
    ([]() {
        return crow::response("高校班级事务管理系统 API 服务正在运行");
    });

    CROW_ROUTE((*app_), "/health")
    ([]() {
        return crow::response(generateSuccessResponse("服务健康检查通过"));
    });
}

void WebServer::setupApiRoutes() {
    // 用户相关API
    CROW_ROUTE((*app_), "/api/v1/users/login").methods("POST"_method)
    ([this](const crow::request& req) {
        return handleUserLogin(req);
    });

    CROW_ROUTE((*app_), "/api/v1/users/register").methods("POST"_method)
    ([this](const crow::request& req) {
        return handleUserRegister(req);
    });

    CROW_ROUTE((*app_), "/api/v1/users/logout").methods("POST"_method)
    ([this](const crow::request& req) {
        return handleUserLogout(req);
    });

    CROW_ROUTE((*app_), "/api/v1/users/<int>").methods("GET"_method)
    ([this](const crow::request& req, int user_id) {
        return handleGetUserInfo(req, user_id);
    });

    CROW_ROUTE((*app_), "/api/v1/users/<int>").methods("PUT"_method)
    ([this](const crow::request& req, int user_id) {
        return handleUpdateUserInfo(req, user_id);
    });

    CROW_ROUTE((*app_), "/api/v1/users/<int>/password").methods("PUT"_method)
    ([this](const crow::request& req, int user_id) {
        return handleChangePassword(req, user_id);
    });

    CROW_ROUTE((*app_), "/api/v1/users").methods("GET"_method)
    ([this](const crow::request& req) {
        return handleGetUsers(req);
    });

    // 通知相关API
    CROW_ROUTE((*app_), "/api/v1/notifications").methods("POST"_method)
    ([this](const crow::request& req) {
        return handlePublishNotification(req);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications/<int>").methods("PUT"_method)
    ([this](const crow::request& req, int notification_id) {
        return handleUpdateNotification(req, notification_id);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications/<int>").methods("DELETE"_method)
    ([this](const crow::request& req, int notification_id) {
        return handleDeleteNotification(req, notification_id);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications").methods("GET"_method)
    ([this](const crow::request& req) {
        return handleGetNotifications(req);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications/<int>").methods("GET"_method)
    ([this](const crow::request& req, int notification_id) {
        return handleGetNotificationById(req, notification_id);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications/<int>/read").methods("POST"_method)
    ([this](const crow::request& req, int notification_id) {
        return handleMarkNotificationRead(req, notification_id);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications/unread").methods("GET"_method)
    ([this](const crow::request& req) {
        return handleGetUnreadNotifications(req);
    });

    CROW_ROUTE((*app_), "/api/v1/notifications/unread/count").methods("GET"_method)
    ([this](const crow::request& req) {
        return handleGetUnreadCount(req);
    });
}

// API处理函数实现
crow::response WebServer::handleUserLogin(const crow::request& req) {
    try {
        crow::json::rvalue json;
        if (!parseJsonRequest(req, json)) {
            return crow::response(400, generateErrorResponse("无效的JSON数据"));
        }

        if (!json.has("username") || !json.has("password")) {
            return crow::response(400, generateErrorResponse("缺少用户名或密码"));
        }

        std::string username = json["username"].s();
        std::string password = json["password"].s();

        auto result = userManager_->login(username, password);

        if (result.success) {
            auto user = userManager_->getUserByUsername(username);
            if (user) {
                crow::json::wvalue response_data;
                response_data["user_id"] = user->id;
                response_data["username"] = user->username;
                response_data["real_name"] = user->real_name;
                response_data["role"] = static_cast<int>(user->role);

                return crow::response(200, generateSuccessResponse("登录成功", response_data.dump()));
            }
        }

        return crow::response(401, generateErrorResponse(result.message));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Login error: %s", e.what());
        return crow::response(500, generateErrorResponse("登录处理失败"));
    }
}

crow::response WebServer::handleUserRegister(const crow::request& req) {
    try {
        crow::json::rvalue json;
        if (!parseJsonRequest(req, json)) {
            return crow::response(400, generateErrorResponse("无效的JSON数据"));
        }

        // 验证必需字段
        std::vector<std::string> required_fields = {"username", "password", "real_name", "class_id"};
        for (const auto& field : required_fields) {
            if (!json.has(field)) {
                return crow::response(400, generateErrorResponse("缺少必需字段: " + field));
            }
        }

        User user;
        user.username = json["username"].s();
        user.real_name = json["real_name"].s();
        user.class_id = json["class_id"].i();

        if (json.has("email")) user.email = json["email"].s();
        if (json.has("phone")) user.phone = json["phone"].s();
        if (json.has("student_id")) user.student_id = json["student_id"].s();
        if (json.has("role")) {
            int role = json["role"].i();
            user.role = static_cast<UserRole>(role);
        }

        // 生成密码哈希
        std::string password = json["password"].s();
        user.salt = CryptoUtils::generateSalt();
        user.password_hash = CryptoUtils::hashPassword(password, user.salt);

        auto result = userManager_->registerUser(user);

        if (result.success) {
            return crow::response(201, generateSuccessResponse("用户注册成功"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Registration error: %s", e.what());
        return crow::response(500, generateErrorResponse("注册处理失败"));
    }
}

crow::response WebServer::handleUserLogout(const crow::request& req) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        // 这里可以实现更复杂的登出逻辑，比如清除session等
        return crow::response(200, generateSuccessResponse("登出成功"));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Logout error: %s", e.what());
        return crow::response(500, generateErrorResponse("登出处理失败"));
    }
}

crow::response WebServer::handleGetUserInfo(const crow::request& req, int user_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        auto user = userManager_->getUserById(user_id);
        if (!user) {
            return crow::response(404, generateErrorResponse("用户不存在"));
        }

        crow::json::wvalue response_data;
        response_data["id"] = user->id;
        response_data["username"] = user->username;
        response_data["real_name"] = user->real_name;
        response_data["email"] = user->email;
        response_data["phone"] = user->phone;
        response_data["role"] = static_cast<int>(user->role);
        response_data["class_id"] = user->class_id;
        response_data["student_id"] = user->student_id;
        response_data["is_active"] = user->is_active;

        return crow::response(200, generateSuccessResponse("获取用户信息成功", response_data.dump()));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Get user info error: %s", e.what());
        return crow::response(500, generateErrorResponse("获取用户信息失败"));
    }
}

crow::response WebServer::handleUpdateUserInfo(const crow::request& req, int user_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        crow::json::rvalue json;
        if (!parseJsonRequest(req, json)) {
            return crow::response(400, generateErrorResponse("无效的JSON数据"));
        }

        auto existingUser = userManager_->getUserById(user_id);
        if (!existingUser) {
            return crow::response(404, generateErrorResponse("用户不存在"));
        }

        // 更新用户信息
        if (json.has("real_name")) existingUser->real_name = json["real_name"].s();
        if (json.has("email")) existingUser->email = json["email"].s();
        if (json.has("phone")) existingUser->phone = json["phone"].s();
        if (json.has("student_id")) existingUser->student_id = json["student_id"].s();
        if (json.has("role")) {
            int role = json["role"].i();
            existingUser->role = static_cast<UserRole>(role);
        }

        auto result = userManager_->updateUser(*existingUser);

        if (result.success) {
            return crow::response(200, generateSuccessResponse("用户信息更新成功"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Update user info error: %s", e.what());
        return crow::response(500, generateErrorResponse("更新用户信息失败"));
    }
}

crow::response WebServer::handleChangePassword(const crow::request& req, int user_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        crow::json::rvalue json;
        if (!parseJsonRequest(req, json)) {
            return crow::response(400, generateErrorResponse("无效的JSON数据"));
        }

        if (!json.has("old_password") || !json.has("new_password")) {
            return crow::response(400, generateErrorResponse("缺少原密码或新密码"));
        }

        std::string old_password = json["old_password"].s();
        std::string new_password = json["new_password"].s();

        auto result = userManager_->changePassword(user_id, old_password, new_password);

        if (result.success) {
            return crow::response(200, generateSuccessResponse("密码修改成功"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Change password error: %s", e.what());
        return crow::response(500, generateErrorResponse("密码修改失败"));
    }
}

crow::response WebServer::handleGetUsers(const crow::request& req) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        // 解析查询参数
        int class_id = 0;
        if (req.url_params.get("class_id") != nullptr) {
            class_id = std::stoi(req.url_params.get("class_id"));
        }

        int role = -1;
        if (req.url_params.get("role") != nullptr) {
            role = std::stoi(req.url_params.get("role"));
        }

        std::vector<User> users;
        if (class_id > 0) {
            users = userManager_->getUsersByClass(class_id);
        } else if (role >= 0) {
            users = userManager_->getUsersByRole(static_cast<UserRole>(role));
        } else {
            return crow::response(400, generateErrorResponse("请指定班级ID或角色"));
        }

        crow::json::wvalue response_data;
        for (size_t i = 0; i < users.size(); ++i) {
            crow::json::wvalue user_json;
            user_json["id"] = users[i].id;
            user_json["username"] = users[i].username;
            user_json["real_name"] = users[i].real_name;
            user_json["email"] = users[i].email;
            user_json["phone"] = users[i].phone;
            user_json["role"] = static_cast<int>(users[i].role);
            user_json["class_id"] = users[i].class_id;
            user_json["student_id"] = users[i].student_id;
            user_json["is_active"] = users[i].is_active;

            response_data["users"][static_cast<int>(i)] = std::move(user_json);
        }

        return crow::response(200, generateSuccessResponse("获取用户列表成功", response_data.dump()));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Get users error: %s", e.what());
        return crow::response(500, generateErrorResponse("获取用户列表失败"));
    }
}

crow::response WebServer::handlePublishNotification(const crow::request& req) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        crow::json::rvalue json;
        if (!parseJsonRequest(req, json)) {
            return crow::response(400, generateErrorResponse("无效的JSON数据"));
        }

        // 验证必需字段
        std::vector<std::string> required_fields = {"title", "content", "class_id"};
        for (const auto& field : required_fields) {
            if (!json.has(field)) {
                return crow::response(400, generateErrorResponse("缺少必需字段: " + field));
            }
        }

        Notification notification;
        notification.title = json["title"].s();
        notification.content = json["content"].s();
        notification.class_id = json["class_id"].i();

        if (json.has("type")) {
            int type = json["type"].i();
            notification.type = static_cast<NotificationType>(type);
        }

        if (json.has("is_important")) {
            notification.is_important = json["is_important"].b();
        }

        // 从认证信息中获取发送者ID
        auto user_id = getUserIdFromToken("dummy_token");
        if (!user_id) {
            return crow::response(401, generateErrorResponse("无法获取用户信息"));
        }
        notification.sender_id = *user_id;

        auto result = notificationManager_->publishNotification(notification);

        if (result.success) {
            return crow::response(201, generateSuccessResponse("通知发布成功"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Publish notification error: %s", e.what());
        return crow::response(500, generateErrorResponse("通知发布失败"));
    }
}

crow::response WebServer::handleUpdateNotification(const crow::request& req, int notification_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        crow::json::rvalue json;
        if (!parseJsonRequest(req, json)) {
            return crow::response(400, generateErrorResponse("无效的JSON数据"));
        }

        auto existingNotification = notificationManager_->getNotificationById(notification_id);
        if (!existingNotification) {
            return crow::response(404, generateErrorResponse("通知不存在"));
        }

        // 更新通知信息
        if (json.has("title")) existingNotification->title = json["title"].s();
        if (json.has("content")) existingNotification->content = json["content"].s();
        if (json.has("type")) {
            int type = json["type"].i();
            existingNotification->type = static_cast<NotificationType>(type);
        }
        if (json.has("is_important")) {
            existingNotification->is_important = json["is_important"].b();
        }

        auto result = notificationManager_->updateNotification(*existingNotification);

        if (result.success) {
            return crow::response(200, generateSuccessResponse("通知更新成功"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Update notification error: %s", e.what());
        return crow::response(500, generateErrorResponse("通知更新失败"));
    }
}

crow::response WebServer::handleDeleteNotification(const crow::request& req, int notification_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        auto result = notificationManager_->deleteNotification(notification_id);

        if (result.success) {
            return crow::response(200, generateSuccessResponse("通知删除成功"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Delete notification error: %s", e.what());
        return crow::response(500, generateErrorResponse("通知删除失败"));
    }
}

crow::response WebServer::handleGetNotifications(const crow::request& req) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        // 解析查询参数
        int class_id = 0;
        if (req.url_params.get("class_id") != nullptr) {
            class_id = std::stoi(req.url_params.get("class_id"));
        }

        int page = 1;
        if (req.url_params.get("page") != nullptr) {
            page = std::stoi(req.url_params.get("page"));
        }

        int page_size = 20;
        if (req.url_params.get("page_size") != nullptr) {
            page_size = std::stoi(req.url_params.get("page_size"));
        }

        Pagination pagination(page, page_size);
        std::vector<Notification> notifications;

        if (class_id > 0) {
            notifications = notificationManager_->getNotificationsByClass(class_id, pagination);
        } else {
            return crow::response(400, generateErrorResponse("请指定班级ID"));
        }

        crow::json::wvalue response_data;
        for (size_t i = 0; i < notifications.size(); ++i) {
            crow::json::wvalue notification_json;
            notification_json["id"] = notifications[i].id;
            notification_json["title"] = notifications[i].title;
            notification_json["content"] = notifications[i].content;
            notification_json["type"] = static_cast<int>(notifications[i].type);
            notification_json["sender_id"] = notifications[i].sender_id;
            notification_json["class_id"] = notifications[i].class_id;
            notification_json["is_important