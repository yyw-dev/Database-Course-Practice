# 数据库设计文档

## 1. 数据库概述

高校班级事务管理系统使用MySQL数据库存储所有业务数据。数据库设计遵循规范化原则，确保数据完整性和查询效率。

## 2. 数据库结构

### 2.1 数据库信息
- **数据库名称**: class_management_system
- **字符集**: utf8mb4
- **排序规则**: utf8mb4_unicode_ci
- **存储引擎**: InnoDB

### 2.2 数据表清单

| 表名 | 描述 | 记录数预估 |
|------|------|-----------|
| classes | 班级信息 | 100-1000 |
| users | 用户信息 | 1000-10000 |
| notifications | 通知信息 | 10000-100000 |
| notification_reads | 通知阅读记录 | 100000-1000000 |
| courses | 课程信息 | 1000-5000 |
| course_schedules | 课程安排 | 5000-20000 |
| grades | 成绩信息 | 10000-100000 |
| finance_records | 班费收支 | 1000-10000 |
| leave_applications | 请假申请 | 5000-50000 |
| activities | 活动信息 | 1000-5000 |
| activity_registrations | 活动报名 | 10000-100000 |
| attendance_records | 考勤记录 | 100000-1000000 |
| study_materials | 学习资料 | 1000-10000 |

## 3. 详细表结构设计

### 3.1 classes (班级表)

| 字段名 | 类型 | 是否为空 | 默认值 | 描述 |
|--------|------|----------|--------|------|
| id | INT | NO | AUTO_INCREMENT | 主键ID |
| name | VARCHAR(100) | NO | | 班级名称 |
| grade | VARCHAR(20) | NO | | 年级 |
| major | VARCHAR(100) | NO | | 专业 |
| student_count | INT | YES | 0 | 学生人数 |
| monitor_id | INT | YES | | 班长ID |
| teacher_id | INT | YES | | 辅导员ID |
| description | TEXT | YES | | 班级描述 |
| is_active | BOOLEAN | YES | TRUE | 是否活跃 |
| created_at | TIMESTAMP | YES | CURRENT_TIMESTAMP | 创建时间 |
| updated_at | TIMESTAMP | YES | CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP | 更新时间 |

**索引**:
- PRIMARY KEY (id)
- INDEX idx_grade (grade)
- INDEX idx_major (major)
- INDEX idx_monitor (monitor_id)
- INDEX idx_teacher (teacher_id)

### 3.2 users (用户表)

| 字段名 | 类型 | 是否为空 | 默认值 | 描述 |
|--------|------|----------|--------|------|
| id | INT | NO | AUTO_INCREMENT | 主键ID |
| username | VARCHAR(50) | NO | | 用户名(唯一) |
| password_hash | VARCHAR(255) | NO | | 密码哈希 |
| salt | VARCHAR(64) | NO | | 密码盐值 |
| real_name | VARCHAR(100) | NO | | 真实姓名 |
| email | VARCHAR(100) | YES | | 邮箱 |
| phone | VARCHAR(20) | YES | | 手机号 |
| role | ENUM | YES | 'student' | 用户角色 |
| class_id | INT | NO | | 班级ID |
| student_id | VARCHAR(20) | YES | | 学号(唯一) |
| is_active | BOOLEAN | YES | TRUE | 是否活跃 |
| last_login_at | TIMESTAMP | YES | NULL | 最后登录时间 |
| created_at | TIMESTAMP | YES | CURRENT_TIMESTAMP | 创建时间 |
| updated_at | TIMESTAMP | YES | CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP | 更新时间 |

**索引**:
- PRIMARY KEY (id)
- UNIQUE KEY idx_username (username)
- INDEX idx_class (class_id)
- INDEX idx_role (role)
- INDEX idx_student_id (student_id)
- FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE

### 3.3 notifications (通知表)

| 字段名 | 类型 | 是否为空 | 默认值 | 描述 |
|--------|------|----------|--------|------|
| id | INT | NO | AUTO_INCREMENT | 主键ID |
| title | VARCHAR(200) | NO | | 通知标题 |
| content | TEXT | NO | | 通知内容 |
| type | ENUM | YES | 'general' | 通知类型 |
| sender_id | INT | NO | | 发送者ID |
| class_id | INT | NO | | 班级ID |
| is_important | BOOLEAN | YES | FALSE | 是否重要 |
| is_archived | BOOLEAN | YES | FALSE | 是否归档 |
| publish_at | TIMESTAMP | YES | NULL | 发布时间 |
| created_at | TIMESTAMP | YES | CURRENT_TIMESTAMP | 创建时间 |
| updated_at | TIMESTAMP | YES | CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP | 更新时间 |

**索引**:
- PRIMARY KEY (id)
- INDEX idx_class (class_id)
- INDEX idx_sender (sender_id)
- INDEX idx_type (type)
- INDEX idx_created_at (created_at)
- FOREIGN KEY (sender_id) REFERENCES users(id) ON DELETE CASCADE
- FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE

### 3.4 notification_reads (通知阅读记录表)

| 字段名 | 类型 | 是否为空 | 默认值 | 描述 |
|--------|------|----------|--------|------|
| id | INT | NO | AUTO_INCREMENT | 主键ID |
| notification_id | INT | NO | | 通知ID |
| user_id | INT | NO | | 用户ID |
| is_read | BOOLEAN | YES | FALSE | 是否已读 |
| read_at | TIMESTAMP | YES | NULL | 阅读时间 |
| created_at | TIMESTAMP | YES | CURRENT_TIMESTAMP | 创建时间 |

**索引**:
- PRIMARY KEY (id)
- UNIQUE KEY unique_notification_user (notification_id, user_id)
- INDEX idx_user (user_id)
- INDEX idx_notification (notification_id)
- FOREIGN KEY (notification_id) REFERENCES notifications(id) ON DELETE CASCADE
- FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE

## 4. 数据库关系图

```
classes (1) ----< (n) users (1) ----< (n) notifications
    |               |                   |
    |               |                   |
    |               v                   v
    |           notification_reads (n)----(1)
    |
    |----< (n) courses ----< (n) course_schedules
    |        |
    |        |----< (n) grades
    |        |----< (n) attendance_records
    |        |----< (n) study_materials
    |
    |----< (n) finance_records
    |----< (n) activities ----< (n) activity_registrations
    |----< (n) leave_applications
```

## 5. 数据库优化策略

### 5.1 索引优化
- 为常用查询条件创建索引
- 使用复合索引优化多条件查询
- 定期分析和优化索引使用

### 5.2 查询优化
- 避免SELECT *，只查询需要的字段
- 使用EXPLAIN分析查询性能
- 合理使用JOIN，避免笛卡尔积

### 5.3 表分区
对于数据量大的表（如attendance_records），考虑按时间分区：
```sql
PARTITION BY RANGE (YEAR(date)) (
    PARTITION p2023 VALUES LESS THAN (2024),
    PARTITION p2024 VALUES LESS THAN (2025),
    PARTITION p_future VALUES LESS THAN MAXVALUE
);
```

## 6. 数据完整性约束

### 6.1 外键约束
所有外键关系都设置了适当的ON DELETE和ON UPDATE规则：
- CASCADE: 级联删除/更新
- SET NULL: 设置为NULL
- RESTRICT: 限制删除/更新

### 6.2 检查约束
使用ENUM类型限制字段取值范围，确保数据一致性。

### 6.3 触发器设计
```sql
-- 自动更新学生人数统计
CREATE TRIGGER update_student_count_insert
AFTER INSERT ON users
FOR EACH ROW
UPDATE classes SET student_count = student_count + 1 WHERE id = NEW.class_id;

CREATE TRIGGER update_student_count_delete
AFTER DELETE ON users
FOR EACH ROW
UPDATE classes SET student_count = student_count - 1 WHERE id = OLD.class_id;
```

## 7. 备份和恢复策略

### 7.1 定期备份
```bash
# 每日全量备份
mysqldump -u username -p class_management_system > backup_$(date +%Y%m%d).sql

# 备份二进制日志
mysqlbinlog /var/lib/mysql/mysql-bin.000001 > binlog_backup.sql
```

### 7.2 恢复策略
```bash
# 从备份恢复
mysql -u username -p class_management_system < backup_file.sql

# 使用二进制日志进行时间点恢复
mysqlbinlog --start-datetime="2024-01-01 00:00:00" binlog_file | mysql -u username -p
```

## 8. 数据库安全

### 8.1 用户权限管理
```sql
-- 创建应用程序用户
CREATE USER 'class_app'@'localhost' IDENTIFIED BY 'strong_password';

-- 授予必要权限
GRANT SELECT, INSERT, UPDATE, DELETE ON class_management_system.* TO 'class_app'@'localhost';

-- 刷新权限
FLUSH PRIVILEGES;
```

### 8.2 数据加密
- 密码使用加盐哈希存储
- 敏感信息考虑使用AES加密
- SSL连接数据库

## 9. 性能监控

### 9.1 慢查询监控
```sql
-- 启用慢查询日志
SET GLOBAL slow_query_log = 'ON';
SET GLOBAL long_query_time = 2;

-- 查看慢查询
SHOW VARIABLES LIKE 'slow_query_log%';
SHOW PROCESSLIST;
```

### 9.2 性能分析
```sql
-- 分析表性能
ANALYZE TABLE users, notifications, grades;

-- 查看索引使用情况
SHOW INDEX FROM users;
EXPLAIN SELECT * FROM users WHERE class_id = 1;
```