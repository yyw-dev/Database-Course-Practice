#include "../../include/core/notification/notification_manager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/validator.h"
#include <sstream>
#include <algorithm>

NotificationManager::NotificationManager() : db_(DatabaseConnector::getInstance()) {
    LOG_INFO("NotificationManager initialized");
}

NotificationManager::~NotificationManager() {
    LOG_INFO("NotificationManager destroyed");
}

Result NotificationManager::publishNotification(const Notification& notification) {
    if (!validateNotificationData(notification)) {
        return Result::Failure("通知数据验证失败");
    }

    return insertNotification(notification);
}

Result NotificationManager::updateNotification(const Notification& notification) {
    if (!validateNotificationData(notification)) {
        return Result::Failure("通知数据验证失败");
    }

    auto existingNotification = getNotificationById(notification.id);
    if (!existingNotification) {
        return Result::Failure("通知不存在");
    }

    return updateNotificationInDB(notification);
}

Result NotificationManager::deleteNotification(int notification_id) {
    auto notification = getNotificationById(notification_id);
    if (!notification) {
        return Result::Failure("通知不存在");
    }

    return deleteNotificationFromDB(notification_id);
}

Result NotificationManager::archiveNotification(int notification_id) {
    auto notification = getNotificationById(notification_id);
    if (!notification) {
        return Result::Failure("通知不存在");
    }

    std::stringstream query;
    query << "UPDATE notifications SET is_archived = TRUE WHERE id = " << notification_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Notification archived: ID %d", notification_id);
        return Result::Success("通知归档成功");
    } else {
        LOG_ERROR_FMT("Failed to archive notification: ID %d", notification_id);
        return Result::Failure("通知归档失败");
    }
}

std::optional<Notification> NotificationManager::getNotificationById(int notification_id) {
    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.id = " << notification_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");

            std::string typeStr = dbResult.getString("type");
            if (typeStr == "general") notification.type = NotificationType::GENERAL;
            else if (typeStr == "urgent") notification.type = NotificationType::URGENT;
            else if (typeStr == "activity") notification.type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") notification.type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") notification.type = NotificationType::FINANCE;
            else notification.type = NotificationType::GENERAL;

            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            // 转换时间戳
            std::string createdAtStr = dbResult.getString("created_at");
            std::string updatedAtStr = dbResult.getString("updated_at");
            if (!createdAtStr.empty()) {
                notification.created_at = TimeUtils::parseTime(createdAtStr);
            }
            if (!updatedAtStr.empty()) {
                notification.updated_at = TimeUtils::parseTime(updatedAtStr);
            }

            return notification;
        }
    }

    return std::nullopt;
}

std::vector<Notification> NotificationManager::getNotificationsByClass(int class_id, const Pagination& pagination) {
    std::vector<Notification> notifications;

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.class_id = " << class_id << " AND n.is_archived = FALSE "
          << "ORDER BY n.is_important DESC, n.created_at DESC "
          << "LIMIT " << pagination.page_size << " OFFSET " << (pagination.page - 1) * pagination.page_size;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");

            std::string typeStr = dbResult.getString("type");
            if (typeStr == "general") notification.type = NotificationType::GENERAL;
            else if (typeStr == "urgent") notification.type = NotificationType::URGENT;
            else if (typeStr == "activity") notification.type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") notification.type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") notification.type = NotificationType::FINANCE;
            else notification.type = NotificationType::GENERAL;

            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            notifications.push_back(notification);
        }
    }

    return notifications;
}

std::vector<Notification> NotificationManager::getNotificationsBySender(int sender_id, const Pagination& pagination) {
    std::vector<Notification> notifications;

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.sender_id = " << sender_id << " "
          << "ORDER BY n.created_at DESC "
          << "LIMIT " << pagination.page_size << " OFFSET " << (pagination.page - 1) * pagination.page_size;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");

            std::string typeStr = dbResult.getString("type");
            if (typeStr == "general") notification.type = NotificationType::GENERAL;
            else if (typeStr == "urgent") notification.type = NotificationType::URGENT;
            else if (typeStr == "activity") notification.type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") notification.type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") notification.type = NotificationType::FINANCE;
            else notification.type = NotificationType::GENERAL;

            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            notifications.push_back(notification);
        }
    }

    return notifications;
}

std::vector<Notification> NotificationManager::getNotificationsByType(NotificationType type, const Pagination& pagination) {
    std::vector<Notification> notifications;

    std::string typeStr;
    switch (type) {
        case NotificationType::GENERAL: typeStr = "general"; break;
        case NotificationType::URGENT: typeStr = "urgent"; break;
        case NotificationType::ACTIVITY: typeStr = "activity"; break;
        case NotificationType::ACADEMIC: typeStr = "academic"; break;
        case NotificationType::FINANCE: typeStr = "finance"; break;
    }

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.type = '" << typeStr << "' AND n.is_archived = FALSE "
          << "ORDER BY n.created_at DESC "
          << "LIMIT " << pagination.page_size << " OFFSET " << (pagination.page - 1) * pagination.page_size;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");
            notification.type = type;
            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            notifications.push_back(notification);
        }
    }

    return notifications;
}

std::vector<Notification> NotificationManager::getImportantNotifications(int class_id) {
    std::vector<Notification> notifications;

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.class_id = " << class_id << " AND n.is_important = TRUE AND n.is_archived = FALSE "
          << "ORDER BY n.created_at DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");

            std::string typeStr = dbResult.getString("type");
            if (typeStr == "general") notification.type = NotificationType::GENERAL;
            else if (typeStr == "urgent") notification.type = NotificationType::URGENT;
            else if (typeStr == "activity") notification.type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") notification.type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") notification.type = NotificationType::FINANCE;
            else notification.type = NotificationType::GENERAL;

            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            notifications.push_back(notification);
        }
    }

    return notifications;
}

std::vector<Notification> NotificationManager::getArchivedNotifications(int class_id, const Pagination& pagination) {
    std::vector<Notification> notifications;

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.class_id = " << class_id << " AND n.is_archived = TRUE "
          << "ORDER BY n.created_at DESC "
          << "LIMIT " << pagination.page_size << " OFFSET " << (pagination.page - 1) * pagination.page_size;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");

            std::string typeStr = dbResult.getString("type");
            if (typeStr == "general") notification.type = NotificationType::GENERAL;
            else if (typeStr == "urgent") notification.type = NotificationType::URGENT;
            else if (typeStr == "activity") notification.type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") notification.type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") notification.type = NotificationType::FINANCE;
            else notification.type = NotificationType::GENERAL;

            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            notifications.push_back(notification);
        }
    }

    return notifications;
}

std::vector<Notification> NotificationManager::searchNotifications(const std::string& keyword, int class_id) {
    std::vector<Notification> notifications;

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "WHERE n.is_archived = FALSE ";

    if (class_id > 0) {
        query << "AND n.class_id = " << class_id << " ";
    }

    query << "AND (n.title LIKE '%" << keyword << "%' OR n.content LIKE '%" << keyword << "%') "
          << "ORDER BY n.created_at DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            Notification notification;
            notification.id = dbResult.getInt("id");
            notification.title = dbResult.getString("title");
            notification.content = dbResult.getString("content");

            std::string typeStr = dbResult.getString("type");
            if (typeStr == "general") notification.type = NotificationType::GENERAL;
            else if (typeStr == "urgent") notification.type = NotificationType::URGENT;
            else if (typeStr == "activity") notification.type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") notification.type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") notification.type = NotificationType::FINANCE;
            else notification.type = NotificationType::GENERAL;

            notification.sender_id = dbResult.getInt("sender_id");
            notification.class_id = dbResult.getInt("class_id");
            notification.is_important = dbResult.getBool("is_important");
            notification.is_archived = dbResult.getBool("is_archived");

            notifications.push_back(notification);
        }
    }

    return notifications;
}