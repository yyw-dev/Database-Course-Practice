#include "../../include/web/server.h"
#include "../../include/utils/logger.h"

// 继续WebServer类的实现

// 辅助函数实现
std::string WebServer::generateResponse(bool success, const std::string& message, const std::string& data) {
    crow::json::wvalue response;
    response["success"] = success;
    response["message"] = message;
    response["timestamp"] = TimeUtils::getCurrentTimeString();

    if (!data.empty()) {
        response["data"] = data;
    }

    return response.dump();
}

std::string WebServer::generateErrorResponse(const std::string& message) {
    return generateResponse(false, message);
}

std::string WebServer::generateSuccessResponse(const std::string& message, const std::string& data) {
    return generateResponse(true, message, data);
}

bool WebServer::authenticateRequest(const crow::request& req) {
    // 简单的认证检查（实际项目中应该使用更复杂的认证机制）
    auto auth_header = req.get_header_value("Authorization");
    return !auth_header.empty();
}

bool WebServer::checkPermission(const crow::request& req, const std::string& permission) {
    // 简单的权限检查（实际项目中应该使用更复杂的权限机制）
    return authenticateRequest(req);
}

std::optional<int> WebServer::getUserIdFromToken(const std::string& token) {
    // 简单的token解析（实际项目中应该使用JWT等标准）
    return 1; // 返回示例用户ID
}

bool WebServer::parseJsonRequest(const crow::request& req, crow::json::rvalue& json) {
    try {
        json = crow::json::load(req.body);
        return json;
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("JSON parse error: %s", e.what());
        return false;
    }
}

// 继续通知相关的API处理函数
crow::response WebServer::handleGetNotificationById(const crow::request& req, int notification_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        auto notification = notificationManager_->getNotificationById(notification_id);
        if (!notification) {
            return crow::response(404, generateErrorResponse("通知不存在"));
        }

        crow::json::wvalue response_data;
        response_data["id"] = notification->id;
        response_data["title"] = notification->title;
        response_data["content"] = notification->content;
        response_data["type"] = static_cast<int>(notification->type);
        response_data["sender_id"] = notification->sender_id;
        response_data["class_id"] = notification->class_id;
        response_data["is_important"] = notification->is_important;
        response_data["is_archived"] = notification->is_archived;

        return crow::response(200, generateSuccessResponse("获取通知详情成功", response_data.dump()));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Get notification by ID error: %s", e.what());
        return crow::response(500, generateErrorResponse("获取通知详情失败"));
    }
}

crow::response WebServer::handleMarkNotificationRead(const crow::request& req, int notification_id) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        auto user_id = getUserIdFromToken("dummy_token");
        if (!user_id) {
            return crow::response(401, generateErrorResponse("无法获取用户信息"));
        }

        auto result = notificationManager_->markAsRead(notification_id, *user_id);

        if (result.success) {
            return crow::response(200, generateSuccessResponse("通知已标记为已读"));
        } else {
            return crow::response(400, generateErrorResponse(result.message));
        }

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Mark notification read error: %s", e.what());
        return crow::response(500, generateErrorResponse("标记通知已读失败"));
    }
}

crow::response WebServer::handleGetUnreadNotifications(const crow::request& req) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        auto user_id = getUserIdFromToken("dummy_token");
        if (!user_id) {
            return crow::response(401, generateErrorResponse("无法获取用户信息"));
        }

        int class_id = 0;
        if (req.url_params.get("class_id") != nullptr) {
            class_id = std::stoi(req.url_params.get("class_id"));
        }

        auto notifications = notificationManager_->getUnreadNotifications(*user_id, class_id);

        crow::json::wvalue response_data;
        for (size_t i = 0; i < notifications.size(); ++i) {
            crow::json::wvalue notification_json;
            notification_json["id"] = notifications[i].id;
            notification_json["title"] = notifications[i].title;
            notification_json["content"] = notifications[i].content;
            notification_json["type"] = static_cast<int>(notifications[i].type);
            notification_json["sender_id"] = notifications[i].sender_id;
            notification_json["class_id"] = notifications[i].class_id;
            notification_json["is_important"] = notifications[i].is_important;

            response_data["notifications"][static_cast<int>(i)] = std::move(notification_json);
        }

        return crow::response(200, generateSuccessResponse("获取未读通知成功", response_data.dump()));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Get unread notifications error: %s", e.what());
        return crow::response(500, generateErrorResponse("获取未读通知失败"));
    }
}

crow::response WebServer::handleGetUnreadCount(const crow::request& req) {
    try {
        if (!authenticateRequest(req)) {
            return crow::response(401, generateErrorResponse("未授权访问"));
        }

        auto user_id = getUserIdFromToken("dummy_token");
        if (!user_id) {
            return crow::response(401, generateErrorResponse("无法获取用户信息"));
        }

        int class_id = 0;
        if (req.url_params.get("class_id") != nullptr) {
            class_id = std::stoi(req.url_params.get("class_id"));
        }

        int count = notificationManager_->getUnreadCount(*user_id, class_id);

        crow::json::wvalue response_data;
        response_data["count"] = count;

        return crow::response(200, generateSuccessResponse("获取未读通知数量成功", response_data.dump()));

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Get unread count error: %s", e.what());
        return crow::response(500, generateErrorResponse("获取未读通知数量失败"));
    }
}