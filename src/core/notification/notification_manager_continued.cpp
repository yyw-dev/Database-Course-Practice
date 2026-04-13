#include "../../include/core/notification/notification_manager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/validator.h"
#include <sstream>
#include <algorithm>

// 通知阅读状态管理
Result NotificationManager::markAsRead(int notification_id, int user_id) {
    // 检查是否已经阅读过
    if (isNotificationRead(notification_id, user_id)) {
        return Result::Success("通知已标记为已读");
    }

    NotificationRead read_status;
    read_status.notification_id = notification_id;
    read_status.user_id = user_id;
    read_status.is_read = true;
    read_status.read_at = TimeUtils::getCurrentTime();

    return insertOrUpdateReadStatus(read_status);
}

Result NotificationManager::markAsUnread(int notification_id, int user_id) {
    std::stringstream query;
    query << "UPDATE notification_reads SET is_read = FALSE, read_at = NULL "
          << "WHERE notification_id = " << notification_id << " AND user_id = " << user_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Notification marked as unread: notification_id=%d, user_id=%d", notification_id, user_id);
        return Result::Success("通知已标记为未读");
    } else {
        LOG_ERROR_FMT("Failed to mark notification as unread: notification_id=%d, user_id=%d", notification_id, user_id);
        return Result::Failure("标记未读失败");
    }
}

Result NotificationManager::markAllAsRead(int user_id, int class_id) {
    std::stringstream query;
    query << "UPDATE notification_reads nr "
          << "JOIN notifications n ON nr.notification_id = n.id "
          << "SET nr.is_read = TRUE, nr.read_at = NOW() "
          << "WHERE nr.user_id = " << user_id << " AND nr.is_read = FALSE";

    if (class_id > 0) {
        query << " AND n.class_id = " << class_id;
    }

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("All notifications marked as read for user_id=%d", user_id);
        return Result::Success("所有通知已标记为已读");
    } else {
        LOG_ERROR_FMT("Failed to mark all notifications as read for user_id=%d", user_id);
        return Result::Failure("批量标记已读失败");
    }
}

bool NotificationManager::isNotificationRead(int notification_id, int user_id) {
    auto read_status = getReadStatusFromDB(notification_id, user_id);
    return read_status && read_status->is_read;
}

std::vector<NotificationRead> NotificationManager::getReadStatus(int notification_id) {
    std::vector<NotificationRead> read_statuses;

    std::stringstream query;
    query << "SELECT * FROM notification_reads WHERE notification_id = " << notification_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            NotificationRead read_status;
            read_status.id = dbResult.getInt("id");
            read_status.notification_id = dbResult.getInt("notification_id");
            read_status.user_id = dbResult.getInt("user_id");
            read_status.is_read = dbResult.getBool("is_read");

            std::string readAtStr = dbResult.getString("read_at");
            if (!readAtStr.empty()) {
                read_status.read_at = TimeUtils::parseTime(readAtStr);
            }

            read_statuses.push_back(read_status);
        }
    }

    return read_statuses;
}

std::vector<Notification> NotificationManager::getUnreadNotifications(int user_id, int class_id) {
    std::vector<Notification> notifications;

    std::stringstream query;
    query << "SELECT n.*, u.real_name as sender_name FROM notifications n "
          << "LEFT JOIN users u ON n.sender_id = u.id "
          << "LEFT JOIN notification_reads nr ON n.id = nr.notification_id AND nr.user_id = " << user_id << " "
          << "WHERE n.is_archived = FALSE AND (nr.is_read = FALSE OR nr.is_read IS NULL)";

    if (class_id > 0) {
        query << " AND n.class_id = " << class_id;
    }

    query << " ORDER BY n.is_important DESC, n.created_at DESC";

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

int NotificationManager::getUnreadCount(int user_id, int class_id) {
    std::stringstream query;
    query << "SELECT COUNT(*) as count FROM notifications n "
          << "LEFT JOIN notification_reads nr ON n.id = nr.notification_id AND nr.user_id = " << user_id << " "
          << "WHERE n.is_archived = FALSE AND (nr.is_read = FALSE OR nr.is_read IS NULL)";

    if (class_id > 0) {
        query << " AND n.class_id = " << class_id;
    }

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            return dbResult.getInt("count");
        }
    }

    return 0;
}

std::map<NotificationType, int> NotificationManager::getNotificationStats(int class_id) {
    std::map<NotificationType, int> stats;

    std::stringstream query;
    query << "SELECT type, COUNT(*) as count FROM notifications "
          << "WHERE class_id = " << class_id << " AND is_archived = FALSE "
          << "GROUP BY type";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            std::string typeStr = dbResult.getString("type");
            int count = dbResult.getInt("count");

            NotificationType type;
            if (typeStr == "general") type = NotificationType::GENERAL;
            else if (typeStr == "urgent") type = NotificationType::URGENT;
            else if (typeStr == "activity") type = NotificationType::ACTIVITY;
            else if (typeStr == "academic") type = NotificationType::ACADEMIC;
            else if (typeStr == "finance") type = NotificationType::FINANCE;
            else type = NotificationType::GENERAL;

            stats[type] = count;
        }
    }

    return stats;
}

std::map<std::string, int> NotificationManager::getNotificationTrends(int class_id, int days) {
    std::map<std::string, int> trends;

    std::stringstream query;
    query << "SELECT DATE(created_at) as date, COUNT(*) as count FROM notifications "
          << "WHERE class_id = " << class_id << " AND created_at >= DATE_SUB(NOW(), INTERVAL " << days << " DAY) "
          << "GROUP BY DATE(created_at) ORDER BY date";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            std::string date = dbResult.getString("date");
            int count = dbResult.getInt("count");
            trends[date] = count;
        }
    }

    return trends;
}

Result NotificationManager::batchPublishNotifications(const std::vector<Notification>& notifications) {
    if (notifications.empty()) {
        return Result::Failure("通知列表为空");
    }

    if (!db_.beginTransaction()) {
        return Result::Failure("开始事务失败");
    }

    bool success = true;
    for (const auto& notification : notifications) {
        if (!validateNotificationData(notification)) {
            success = false;
            break;
        }

        if (!insertNotification(notification).success) {
            success = false;
            break;
        }
    }

    if (success) {
        if (db_.commitTransaction()) {
            LOG_INFO_FMT("Batch published %zu notifications successfully", notifications.size());
            return Result::Success("批量发布通知成功");
        } else {
            db_.rollbackTransaction();
            return Result::Failure("提交事务失败");
        }
    } else {
        db_.rollbackTransaction();
        return Result::Failure("批量发布通知失败");
    }
}

Result NotificationManager::batchDeleteNotifications(const std::vector<int>& notification_ids) {
    if (notification_ids.empty()) {
        return Result::Failure("通知ID列表为空");
    }

    if (!db_.beginTransaction()) {
        return Result::Failure("开始事务失败");
    }

    bool success = true;
    for (int notification_id : notification_ids) {
        if (!deleteNotificationFromDB(notification_id).success) {
            success = false;
            break;
        }
    }

    if (success) {
        if (db_.commitTransaction()) {
            LOG_INFO_FMT("Batch deleted %zu notifications successfully", notification_ids.size());
            return Result::Success("批量删除通知成功");
        } else {
            db_.rollbackTransaction();
            return Result::Failure("提交事务失败");
        }
    } else {
        db_.rollbackTransaction();
        return Result::Failure("批量删除通知失败");
    }
}

Result NotificationManager::batchArchiveNotifications(const std::vector<int>& notification_ids) {
    if (notification_ids.empty()) {
        return Result::Failure("通知ID列表为空");
    }

    if (!db_.beginTransaction()) {
        return Result::Failure("开始事务失败");
    }

    std::stringstream query;
    query << "UPDATE notifications SET is_archived = TRUE WHERE id IN (";
    for (size_t i = 0; i < notification_ids.size(); ++i) {
        if (i > 0) query << ",";
        query << notification_ids[i];
    }
    query << ")";

    bool success = db_.execute(query.str());

    if (success) {
        if (db_.commitTransaction()) {
            LOG_INFO_FMT("Batch archived %zu notifications successfully", notification_ids.size());
            return Result::Success("批量归档通知成功");
        } else {
            db_.rollbackTransaction();
            return Result::Failure("提交事务失败");
        }
    } else {
        db_.rollbackTransaction();
        return Result::Failure("批量归档通知失败");
    }
}

// 私有方法实现
bool NotificationManager::validateNotificationData(const Notification& notification) {
    if (!Validator::validateNotificationTitle(notification.title)) {
        return false;
    }

    if (!Validator::validateNotificationContent(notification.content)) {
        return false;
    }

    if (notification.sender_id <= 0 || notification.class_id <= 0) {
        return false;
    }

    return true;
}

Result NotificationManager::insertNotification(const Notification& notification) {
    std::stringstream query;
    query << "INSERT INTO notifications (title, content, type, sender_id, class_id, is_important, publish_at) VALUES ("
          << "'" << Validator::sanitizeString(notification.title) << "', "
          << "'" << Validator::sanitizeString(notification.content) << "', "
          << "'";

    switch (notification.type) {
        case NotificationType::GENERAL: query << "general"; break;
        case NotificationType::URGENT: query << "urgent"; break;
        case NotificationType::ACTIVITY: query << "activity"; break;
        case NotificationType::ACADEMIC: query << "academic"; break;
        case NotificationType::FINANCE: query << "finance"; break;
    }

    query << "', "
          << notification.sender_id << ", "
          << notification.class_id << ", "
          << (notification.is_important ? "TRUE" : "FALSE") << ", "
          << "NOW()"
          << ")";

    if (db_.execute(query.str())) {
        int notification_id = mysql_insert_id(db_.getConnection());
        LOG_INFO_FMT("Notification created successfully: ID %d, title: %s", notification_id, notification.title.c_str());

        // 自动为班级所有成员创建阅读记录
        sendNotificationToUsers(notification, getUserIdsByClass(notification.class_id));

        return Result::Success("通知发布成功");
    } else {
        LOG_ERROR_FMT("Failed to create notification: %s", notification.title.c_str());
        return Result::Failure("通知发布失败: " + db_.getLastError());
    }
}

Result NotificationManager::updateNotificationInDB(const Notification& notification) {
    std::stringstream query;
    query << "UPDATE notifications SET "
          << "title = '" << Validator::sanitizeString(notification.title) << "', "
          << "content = '" << Validator::sanitizeString(notification.content) << "', "
          << "type = '";

    switch (notification.type) {
        case NotificationType::GENERAL: query << "general"; break;
        case NotificationType::URGENT: query << "urgent"; break;
        case NotificationType::ACTIVITY: query << "activity"; break;
        case NotificationType::ACADEMIC: query << "academic"; break;
        case NotificationType::FINANCE: query << "finance"; break;
    }

    query << "', "
          << "is_important = " << (notification.is_important ? "TRUE" : "FALSE")
          << " WHERE id = " << notification.id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Notification updated successfully: ID %d", notification.id);
        return Result::Success("通知更新成功");
    } else {
        LOG_ERROR_FMT("Failed to update notification: ID %d", notification.id);
        return Result::Failure("通知更新失败: " + db_.getLastError());
    }
}

Result NotificationManager::deleteNotificationFromDB(int notification_id) {
    std::stringstream query;
    query << "DELETE FROM notifications WHERE id = " << notification_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Notification deleted successfully: ID %d", notification_id);
        return Result::Success("通知删除成功");
    } else {
        LOG_ERROR_FMT("Failed to delete notification: ID %d", notification_id);
        return Result::Failure("通知删除失败: " + db_.getLastError());
    }
}

std::optional<Notification> NotificationManager::loadNotificationFromDB(int notification_id) {
    return getNotificationById(notification_id);
}

Result NotificationManager::insertOrUpdateReadStatus(const NotificationRead& read_status) {
    // 先检查记录是否存在
    auto existing = getReadStatusFromDB(read_status.notification_id, read_status.user_id);

    std::stringstream query;
    if (existing) {
        // 更新现有记录
        query << "UPDATE notification_reads SET is_read = " << (read_status.is_read ? "TRUE" : "FALSE")
              << ", read_at = NOW() WHERE notification_id = " << read_status.notification_id
              << " AND user_id = " << read_status.user_id;
    } else {
        // 插入新记录
        query << "INSERT INTO notification_reads (notification_id, user_id, is_read, read_at) VALUES ("
              << read_status.notification_id << ", " << read_status.user_id << ", "
              << (read_status.is_read ? "TRUE" : "FALSE") << ", NOW())";
    }

    if (db_.execute(query.str())) {
        return Result::Success("阅读状态更新成功");
    } else {
        return Result::Failure("阅读状态更新失败: " + db_.getLastError());
    }
}

std::optional<NotificationRead> NotificationManager::getReadStatusFromDB(int notification_id, int user_id) {
    std::stringstream query;
    query << "SELECT * FROM notification_reads WHERE notification_id = " << notification_id
          << " AND user_id = " << user_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            NotificationRead read_status;
            read_status.id = dbResult.getInt("id");
            read_status.notification_id = dbResult.getInt("notification_id");
            read_status.user_id = dbResult.getInt("user_id");
            read_status.is_read = dbResult.getBool("is_read");

            std::string readAtStr = dbResult.getString("read_at");
            if (!readAtStr.empty()) {
                read_status.read_at = TimeUtils::parseTime(readAtStr);
            }

            return read_status;
        }
    }

    return std::nullopt;
}

void NotificationManager::sendNotificationToUsers(const Notification& notification, const std::vector<int>& user_ids) {
    if (user_ids.empty()) return;

    std::stringstream query;
    query << "INSERT INTO notification_reads (notification_id, user_id, is_read) VALUES ";

    for (size_t i = 0; i < user_ids.size(); ++i) {
        if (i > 0) query << ", ";
        query << "(" << notification.id << ", " << user_ids[i] << ", FALSE)";
    }

    if (!db_.execute(query.str())) {
        LOG_ERROR("Failed to create notification read records");
    }
}

std::vector<int> NotificationManager::getUserIdsByClass(int class_id) {
    std::vector<int> user_ids;

    std::stringstream query;
    query << "SELECT id FROM users WHERE class_id = " << class_id << " AND is_active = TRUE";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            user_ids.push_back(dbResult.getInt("id"));
        }
    }

    return user_ids;
}