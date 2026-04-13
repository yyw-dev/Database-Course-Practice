#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "../common.h"
#include "../../database/database_connector.h"

// 通知信息结构
struct Notification {
    int id = 0;
    std::string title;
    std::string content;
    NotificationType type = NotificationType::GENERAL;
    int sender_id = 0;
    int class_id = 0;
    bool is_important = false;
    bool is_archived = false;
    TimePoint created_at;
    TimePoint updated_at;

    // 转换为Map便于JSON序列化
    StringMap toMap() const;
    static Notification fromMap(const StringMap& map);
};

// 通知阅读记录结构
struct NotificationRead {
    int id = 0;
    int notification_id = 0;
    int user_id = 0;
    bool is_read = false;
    TimePoint read_at;
    TimePoint created_at;

    StringMap toMap() const;
    static NotificationRead fromMap(const StringMap& map);
};

// 通知管理器类
class NotificationManager {
public:
    NotificationManager();
    ~NotificationManager();

    // 通知发布和管理
    Result publishNotification(const Notification& notification);
    Result updateNotification(const Notification& notification);
    Result deleteNotification(int notification_id);
    Result archiveNotification(int notification_id);

    // 通知查询
    std::optional<Notification> getNotificationById(int notification_id);
    std::vector<Notification> getNotificationsByClass(int class_id, const Pagination& pagination = Pagination());
    std::vector<Notification> getNotificationsBySender(int sender_id, const Pagination& pagination = Pagination());
    std::vector<Notification> getNotificationsByType(NotificationType type, const Pagination& pagination = Pagination());
    std::vector<Notification> getImportantNotifications(int class_id);
    std::vector<Notification> getArchivedNotifications(int class_id, const Pagination& pagination = Pagination());

    // 通知搜索
    std::vector<Notification> searchNotifications(const std::string& keyword, int class_id = 0);

    // 通知阅读状态管理
    Result markAsRead(int notification_id, int user_id);
    Result markAsUnread(int notification_id, int user_id);
    Result markAllAsRead(int user_id, int class_id = 0);

    // 阅读状态查询
    bool isNotificationRead(int notification_id, int user_id);
    std::vector<NotificationRead> getReadStatus(int notification_id);
    std::vector<Notification> getUnreadNotifications(int user_id, int class_id = 0);
    int getUnreadCount(int user_id, int class_id = 0);

    // 通知统计
    std::map<NotificationType, int> getNotificationStats(int class_id);
    std::map<std::string, int> getNotificationTrends(int class_id, int days = 30);

    // 批量操作
    Result batchPublishNotifications(const std::vector<Notification>& notifications);
    Result batchDeleteNotifications(const std::vector<int>& notification_ids);
    Result batchArchiveNotifications(const std::vector<int>& notification_ids);

private:
    DatabaseConnector& db_;

    // 内部辅助方法
    bool validateNotificationData(const Notification& notification);
    void sendNotificationToUsers(const Notification& notification, const std::vector<int>& user_ids);

    // 数据库操作
    Result insertNotification(const Notification& notification);
    Result updateNotificationInDB(const Notification& notification);
    Result deleteNotificationFromDB(int notification_id);
    std::optional<Notification> loadNotificationFromDB(int notification_id);

    // 阅读状态数据库操作
    Result insertOrUpdateReadStatus(const NotificationRead& read_status);
    std::optional<NotificationRead> getReadStatusFromDB(int notification_id, int user_id);
};

#endif // NOTIFICATION_MANAGER_H