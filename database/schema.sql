-- 高校班级事务管理系统数据库Schema

-- 创建数据库
CREATE DATABASE IF NOT EXISTS class_management_system;
USE class_management_system;

-- 班级表
CREATE TABLE classes (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL COMMENT '班级名称',
    grade VARCHAR(20) NOT NULL COMMENT '年级',
    major VARCHAR(100) NOT NULL COMMENT '专业',
    student_count INT DEFAULT 0 COMMENT '学生人数',
    monitor_id INT COMMENT '班长ID',
    teacher_id INT COMMENT '辅导员ID',
    description TEXT COMMENT '班级描述',
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_grade (grade),
    INDEX idx_major (major),
    INDEX idx_monitor (monitor_id),
    INDEX idx_teacher (teacher_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 用户表
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL COMMENT '用户名',
    password_hash VARCHAR(255) NOT NULL COMMENT '密码哈希',
    salt VARCHAR(64) NOT NULL COMMENT '密码盐值',
    real_name VARCHAR(100) NOT NULL COMMENT '真实姓名',
    email VARCHAR(100) COMMENT '邮箱',
    phone VARCHAR(20) COMMENT '手机号',
    role ENUM('student', 'class_monitor', 'study_monitor', 'life_monitor', 'sports_monitor', 'teacher', 'admin') DEFAULT 'student' COMMENT '角色',
    class_id INT NOT NULL COMMENT '班级ID',
    student_id VARCHAR(20) UNIQUE COMMENT '学号',
    is_active BOOLEAN DEFAULT TRUE,
    last_login_at TIMESTAMP NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE,
    INDEX idx_username (username),
    INDEX idx_class (class_id),
    INDEX idx_role (role),
    INDEX idx_student_id (student_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 通知表
CREATE TABLE notifications (
    id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(200) NOT NULL COMMENT '通知标题',
    content TEXT NOT NULL COMMENT '通知内容',
    type ENUM('general', 'urgent', 'activity', 'academic', 'finance') DEFAULT 'general' COMMENT '通知类型',
    sender_id INT NOT NULL COMMENT '发送者ID',
    class_id INT NOT NULL COMMENT '班级ID',
    is_important BOOLEAN DEFAULT FALSE COMMENT '是否重要',
    is_archived BOOLEAN DEFAULT FALSE COMMENT '是否归档',
    publish_at TIMESTAMP NULL COMMENT '发布时间',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (sender_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE,
    INDEX idx_class (class_id),
    INDEX idx_sender (sender_id),
    INDEX idx_type (type),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 通知阅读记录表
CREATE TABLE notification_reads (
    id INT PRIMARY KEY AUTO_INCREMENT,
    notification_id INT NOT NULL,
    user_id INT NOT NULL,
    is_read BOOLEAN DEFAULT FALSE,
    read_at TIMESTAMP NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (notification_id) REFERENCES notifications(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_notification_user (notification_id, user_id),
    INDEX idx_user (user_id),
    INDEX idx_notification (notification_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 课程表
CREATE TABLE courses (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL COMMENT '课程名称',
    code VARCHAR(20) UNIQUE NOT NULL COMMENT '课程代码',
    teacher_id INT NOT NULL COMMENT '教师ID',
    class_id INT NOT NULL COMMENT '班级ID',
    credit DECIMAL(3,1) NOT NULL COMMENT '学分',
    semester VARCHAR(20) NOT NULL COMMENT '学期',
    description TEXT COMMENT '课程描述',
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (teacher_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE,
    INDEX idx_class (class_id),
    INDEX idx_teacher (teacher_id),
    INDEX idx_semester (semester)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 课程表安排表
CREATE TABLE course_schedules (
    id INT PRIMARY KEY AUTO_INCREMENT,
    course_id INT NOT NULL COMMENT '课程ID',
    day_of_week TINYINT NOT NULL COMMENT '星期几(1-7)',
    start_time TIME NOT NULL COMMENT '开始时间',
    end_time TIME NOT NULL COMMENT '结束时间',
    classroom VARCHAR(50) COMMENT '教室',
    week_type ENUM('all', 'odd', 'even') DEFAULT 'all' COMMENT '周次类型',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    INDEX idx_course (course_id),
    INDEX idx_day_time (day_of_week, start_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 成绩表
CREATE TABLE grades (
    id INT PRIMARY KEY AUTO_INCREMENT,
    student_id INT NOT NULL COMMENT '学生ID',
    course_id INT NOT NULL COMMENT '课程ID',
    grade_type ENUM('regular', 'midterm', 'final', 'total') NOT NULL COMMENT '成绩类型',
    score DECIMAL(5,2) NOT NULL COMMENT '分数',
    grade_point DECIMAL(3,2) COMMENT '绩点',
    remarks TEXT COMMENT '备注',
    recorded_by INT NOT NULL COMMENT '录入人',
    recorded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (recorded_by) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_student_course_type (student_id, course_id, grade_type),
    INDEX idx_student (student_id),
    INDEX idx_course (course_id),
    INDEX idx_type (grade_type)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 班费收支表
CREATE TABLE finance_records (
    id INT PRIMARY KEY AUTO_INCREMENT,
    class_id INT NOT NULL COMMENT '班级ID',
    type ENUM('income', 'expense') NOT NULL COMMENT '收支类型',
    category VARCHAR(50) NOT NULL COMMENT '类别',
    amount DECIMAL(10,2) NOT NULL COMMENT '金额',
    description TEXT NOT NULL COMMENT '说明',
    payer VARCHAR(100) COMMENT '付款人/收款人',
    handler_id INT NOT NULL COMMENT '经手人',
    record_date DATE NOT NULL COMMENT '记录日期',
    is_public BOOLEAN DEFAULT TRUE COMMENT '是否公开',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE,
    FOREIGN KEY (handler_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_class (class_id),
    INDEX idx_type (type),
    INDEX idx_date (record_date),
    INDEX idx_handler (handler_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 请假申请表
CREATE TABLE leave_applications (
    id INT PRIMARY KEY AUTO_INCREMENT,
    student_id INT NOT NULL COMMENT '学生ID',
    leave_type ENUM('sick', 'personal', 'emergency', 'other') NOT NULL COMMENT '请假类型',
    start_date DATE NOT NULL COMMENT '开始日期',
    end_date DATE NOT NULL COMMENT '结束日期',
    reason TEXT NOT NULL COMMENT '请假原因',
    status ENUM('pending', 'approved', 'rejected', 'cancelled') DEFAULT 'pending' COMMENT '状态',
    approver_id INT COMMENT '审批人',
    approval_comment TEXT COMMENT '审批意见',
    approval_date TIMESTAMP NULL COMMENT '审批日期',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (approver_id) REFERENCES users(id) ON DELETE SET NULL,
    INDEX idx_student (student_id),
    INDEX idx_status (status),
    INDEX idx_date_range (start_date, end_date)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 活动表
CREATE TABLE activities (
    id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(200) NOT NULL COMMENT '活动标题',
    description TEXT NOT NULL COMMENT '活动描述',
    organizer_id INT NOT NULL COMMENT '组织者',
    class_id INT NOT NULL COMMENT '班级ID',
    activity_type VARCHAR(50) NOT NULL COMMENT '活动类型',
    location VARCHAR(200) COMMENT '活动地点',
    start_time DATETIME NOT NULL COMMENT '开始时间',
    end_time DATETIME NOT NULL COMMENT '结束时间',
    max_participants INT DEFAULT 0 COMMENT '最大参与人数',
    current_participants INT DEFAULT 0 COMMENT '当前参与人数',
    status ENUM('planning', 'registering', 'ongoing', 'completed', 'cancelled') DEFAULT 'planning' COMMENT '状态',
    registration_deadline DATETIME COMMENT '报名截止时间',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (organizer_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (class_id) REFERENCES classes(id) ON DELETE CASCADE,
    INDEX idx_class (class_id),
    INDEX idx_organizer (organizer_id),
    INDEX idx_status (status),
    INDEX idx_start_time (start_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 活动报名表
CREATE TABLE activity_registrations (
    id INT PRIMARY KEY AUTO_INCREMENT,
    activity_id INT NOT NULL,
    user_id INT NOT NULL,
    registration_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status ENUM('registered', 'attended', 'absent', 'cancelled') DEFAULT 'registered' COMMENT '状态',
    notes TEXT COMMENT '备注',
    FOREIGN KEY (activity_id) REFERENCES activities(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_activity_user (activity_id, user_id),
    INDEX idx_activity (activity_id),
    INDEX idx_user (user_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 考勤记录表
CREATE TABLE attendance_records (
    id INT PRIMARY KEY AUTO_INCREMENT,
    student_id INT NOT NULL COMMENT '学生ID',
    course_id INT NOT NULL COMMENT '课程ID',
    date DATE NOT NULL COMMENT '日期',
    status ENUM('present', 'absent', 'late', 'sick_leave', 'personal_leave') NOT NULL COMMENT '考勤状态',
    remarks TEXT COMMENT '备注',
    recorded_by INT NOT NULL COMMENT '记录人',
    recorded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (recorded_by) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_student_course_date (student_id, course_id, date),
    INDEX idx_student (student_id),
    INDEX idx_course (course_id),
    INDEX idx_date (date),
    INDEX idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 学习资料表
CREATE TABLE study_materials (
    id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(200) NOT NULL COMMENT '资料标题',
    description TEXT COMMENT '资料描述',
    file_path VARCHAR(500) NOT NULL COMMENT '文件路径',
    file_size BIGINT NOT NULL COMMENT '文件大小',
    file_type VARCHAR(50) NOT NULL COMMENT '文件类型',
    uploader_id INT NOT NULL COMMENT '上传者',
    course_id INT NOT NULL COMMENT '课程ID',
    download_count INT DEFAULT 0 COMMENT '下载次数',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (uploader_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    INDEX idx_course (course_id),
    INDEX idx_uploader (uploader_id),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;