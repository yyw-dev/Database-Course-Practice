-- 高校班级事务管理系统初始化数据

USE class_management_system;

-- 插入示例班级数据
INSERT INTO classes (name, grade, major, student_count, description) VALUES
('计算机科学与技术2021级1班', '2021', '计算机科学与技术', 30, '计算机学院2021级1班'),
('软件工程2021级1班', '2021', '软件工程', 32, '计算机学院2021级软件工程1班'),
('网络工程2021级1班', '2021', '网络工程', 28, '计算机学院2021级网络工程1班'),
('信息安全2021级1班', '2021', '信息安全', 25, '计算机学院2021级信息安全1班');

-- 插入示例用户数据
-- 密码: admin123 (经过哈希处理)
INSERT INTO users (username, password_hash, salt, real_name, email, phone, role, class_id, student_id) VALUES
('admin', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '系统管理员', 'admin@school.edu.cn', '13800138000', 'admin', 1, 'ADMIN001'),
('teacher001', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '张教授', 'zhang@school.edu.cn', '13800138001', 'teacher', 1, 'T001'),
('student001', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '李同学', 'li@student.edu.cn', '13800138002', 'student', 1, '2021001'),
('student002', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '王同学', 'wang@student.edu.cn', '13800138003', 'student', 1, '2021002'),
('monitor001', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '陈班长', 'chen@student.edu.cn', '13800138004', 'class_monitor', 1, '2021003'),
('student003', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '刘同学', 'liu@student.edu.cn', '13800138005', 'student', 2, '2021004'),
('student004', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'randomsalt12345678901234567890123456789012', '赵同学', 'zhao@student.edu.cn', '13800138006', 'student', 2, '2021005');

-- 更新班级的班长和辅导员信息
UPDATE classes SET monitor_id = 5, teacher_id = 2 WHERE id = 1;
UPDATE classes SET teacher_id = 2 WHERE id = 2;
UPDATE classes SET teacher_id = 2 WHERE id = 3;
UPDATE classes SET teacher_id = 2 WHERE id = 4;

-- 插入示例课程数据
INSERT INTO courses (name, code, teacher_id, class_id, credit, semester, description) VALUES
('数据结构', 'CS1001', 2, 1, 4.0, '2024春季', '数据结构是计算机科学的基础课程'),
('算法分析', 'CS1002', 2, 1, 3.0, '2024春季', '算法设计与分析'),
('数据库原理', 'CS1003', 2, 1, 3.5, '2024春季', '数据库系统原理与应用'),
('软件工程', 'SE1001', 2, 2, 3.0, '2024春季', '软件工程理论与实践'),
('计算机网络', 'NE1001', 2, 3, 3.0, '2024春季', '计算机网络原理与应用');

-- 插入课程安排数据
INSERT INTO course_schedules (course_id, day_of_week, start_time, end_time, classroom) VALUES
(1, 1, '08:00:00', '09:40:00', 'A101'),  -- 周一 8:00-9:40
(1, 3, '10:00:00', '11:40:00', 'A101'),  -- 周三 10:00-11:40
(2, 2, '14:00:00', '15:40:00', 'A102'),  -- 周二 14:00-15:40
(2, 4, '16:00:00', '17:40:00', 'A102'),  -- 周四 16:00-17:40
(3, 1, '19:00:00', '20:40:00', 'B201'),  -- 周一 19:00-20:40
(4, 2, '08:00:00', '09:40:00', 'B202'),  -- 周二 8:00-9:40
(5, 3, '14:00:00', '15:40:00', 'C301');  -- 周三 14:00-15:40

-- 插入示例成绩数据
INSERT INTO grades (student_id, course_id, grade_type, score, grade_point, recorded_by) VALUES
(3, 1, 'regular', 85.0, 3.5, 2),
(3, 1, 'midterm', 88.0, 3.8, 2),
(3, 1, 'final', 92.0, 4.0, 2),
(3, 1, 'total', 89.5, 3.9, 2),
(4, 1, 'regular', 78.0, 2.8, 2),
(4, 1, 'midterm', 82.0, 3.2, 2),
(4, 1, 'final', 85.0, 3.5, 2),
(4, 1, 'total', 82.5, 3.2, 2),
(3, 2, 'regular', 90.0, 4.0, 2),
(3, 2, 'midterm', 87.0, 3.7, 2),
(4, 2, 'regular', 75.0, 2.5, 2),
(4, 2, 'midterm', 80.0, 3.0, 2);

-- 插入示例班费收支数据
INSERT INTO finance_records (class_id, type, category, amount, description, payer, handler_id, record_date) VALUES
(1, 'income', '班费收缴', 1500.00, '2024年上学期班费收缴', '全体同学', 5, '2024-02-20'),
(1, 'expense', '教材购买', 320.50, '购买专业教材', '教材供应商', 5, '2024-02-25'),
(1, 'expense', '活动经费', 180.00, '班级春游活动费用', '活动承办方', 5, '2024-03-15'),
(1, 'income', '活动结余', 50.00, '活动结余退款', '活动承办方', 5, '2024-03-16'),
(2, 'income', '班费收缴', 1600.00, '2024年上学期班费收缴', '全体同学', 1, '2024-02-20');

-- 插入示例通知数据
INSERT INTO notifications (title, content, type, sender_id, class_id, is_important, publish_at) VALUES
('关于2024年春季学期课程安排的通知', '各位同学：\n2024年春季学期的课程安排已经确定，请大家查看课程表并做好准备。\n具体课程安排请查看附件。', 'academic', 2, 1, TRUE, '2024-02-15 09:00:00'),
('班级春游活动通知', '亲爱的同学们：\n为了丰富大家的课余生活，增强班级凝聚力，我们计划在本月底组织一次春游活动。\n时间：3月30日\n地点：市郊公园\n费用：每人60元\n请有意参加的同学在3月25日前报名。', 'activity', 5, 1, FALSE, '2024-03-10 14:30:00'),
('关于班费收支公示的通知', '各位同学：\n现将本学期的班费收支情况进行公示，请大家监督。\n收入：1500元（班费收缴）\n支出：500.50元（教材320.50元+活动180元）\n余额：999.50元', 'finance', 5, 1, FALSE, '2024-03-20 16:00:00'),
('考试安排通知', '各位同学请注意：\n数据结构和算法分析课程的期末考试安排如下：\n数据结构：4月15日 上午9:00-11:00\n算法分析：4月17日 下午2:00-4:00\n请做好复习准备。', 'academic', 2, 1, TRUE, '2024-04-01 10:00:00');

-- 插入通知阅读记录
INSERT INTO notification_reads (notification_id, user_id, is_read, read_at) VALUES
(1, 3, TRUE, '2024-02-15 09:30:00'),
(1, 4, TRUE, '2024-02-15 10:15:00'),
(2, 3, TRUE, '2024-03-10 15:00:00'),
(3, 3, TRUE, '2024-03-20 16:30:00'),
(4, 3, FALSE, NULL);

-- 插入示例请假申请数据
INSERT INTO leave_applications (student_id, leave_type, start_date, end_date, reason, status, approver_id, approval_comment, approval_date) VALUES
(3, 'sick', '2024-03-15', '2024-03-16', '感冒发烧，需要休息两天', 'approved', 2, '同意请假，注意休息', '2024-03-14 18:00:00'),
(4, 'personal', '2024-03-20', '2024-03-20', '家中有事，需要请假一天', 'pending', NULL, NULL, NULL),
(3, 'emergency', '2024-04-05', '2024-04-07', '突发家庭紧急情况', 'approved', 2, '同意请假，处理完事情后及时返校', '2024-04-04 20:00:00');

-- 插入示例活动数据
INSERT INTO activities (title, description, organizer_id, class_id, activity_type, location, start_time, end_time, max_participants, status, registration_deadline) VALUES
('班级春游活动', '春季户外踏青活动，增进同学友谊，放松心情', 5, 1, '户外活动', '市郊公园', '2024-03-30 09:00:00', '2024-03-30 17:00:00', 30, 'completed', '2024-03-25 23:59:59'),
('学术讲座：人工智能前沿技术', '邀请知名专家讲解AI最新发展动态', 2, 1, '学术活动', '学术报告厅', '2024-04-10 14:00:00', '2024-04-10 16:00:00', 100, 'registering', '2024-04-08 23:59:59'),
('班级篮球比赛', '与兄弟班级进行友谊篮球赛', 5, 1, '体育活动', '学校体育馆', '2024-04-20 15:00:00', '2024-04-20 17:00:00', 20, 'planning', '2024-04-18 23:59:59');

-- 插入活动报名数据
INSERT INTO activity_registrations (activity_id, user_id, status, notes) VALUES
(1, 3, 'attended', '准时参加'),
(1, 4, 'attended', '积极参与'),
(1, 5, 'attended', '活动组织者'),
(2, 3, 'registered', '期待讲座'),
(2, 4, 'registered', '对AI技术很感兴趣');

-- 插入考勤记录数据
INSERT INTO attendance_records (student_id, course_id, date, status, remarks, recorded_by) VALUES
(3, 1, '2024-02-20', 'present', '认真听讲', 2),
(3, 1, '2024-02-22', 'present', '积极回答问题', 2),
(3, 1, '2024-02-27', 'late', '迟到5分钟', 2),
(4, 1, '2024-02-20', 'present', '正常出勤', 2),
(4, 1, '2024-02-22', 'absent', '病假', 2),
(4, 1, '2024-02-27', 'present', '补交作业', 2),
(3, 2, '2024-03-05', 'present', '参与讨论', 2),
(4, 2, '2024-03-05', 'present', '认真笔记', 2);

-- 插入学习资料数据
INSERT INTO study_materials (title, description, file_path, file_size, file_type, uploader_id, course_id) VALUES
('数据结构课件第一章', '线性表的基本概念和操作', '/uploads/materials/data_structure_ch1.pdf', 2048000, 'application/pdf', 2, 1),
('算法分析习题集', '算法设计与分析的练习题和答案', '/uploads/materials/algorithms_exercises.pdf', 1536000, 'application/pdf', 2, 2),
('数据库实验指导书', '数据库原理实验操作步骤', '/uploads/materials/db_lab_guide.docx', 3072000, 'application/vnd.openxmlformats-officedocument.wordprocessingml.document', 2, 3);

-- 创建视图：班级学生名单
CREATE VIEW class_student_list AS
SELECT
    c.name as class_name,
    u.real_name as student_name,
    u.student_id,
    u.phone,
    u.email,
    CASE u.role
        WHEN 'class_monitor' THEN '班长'
        WHEN 'study_monitor' THEN '学习委员'
        WHEN 'life_monitor' THEN '生活委员'
        WHEN 'sports_monitor' THEN '体育委员'
        ELSE '学生'
    END as role_name
FROM users u
JOIN classes c ON u.class_id = c.id
WHERE u.role IN ('student', 'class_monitor', 'study_monitor', 'life_monitor', 'sports_monitor')
ORDER BY c.name, u.student_id;

-- 创建视图：班费收支汇总
CREATE VIEW finance_summary AS
SELECT
    c.name as class_name,
    SUM(CASE WHEN fr.type = 'income' THEN fr.amount ELSE 0 END) as total_income,
    SUM(CASE WHEN fr.type = 'expense' THEN fr.amount ELSE 0 END) as total_expense,
    SUM(CASE WHEN fr.type = 'income' THEN fr.amount ELSE -fr.amount END) as balance
FROM classes c
LEFT JOIN finance_records fr ON c.id = fr.class_id
GROUP BY c.id, c.name;

-- 创建视图：学生成绩汇总
CREATE VIEW student_grades_summary AS
SELECT
    u.real_name as student_name,
    c.name as class_name,
    co.name as course_name,
    MAX(CASE WHEN g.grade_type = 'total' THEN g.score ELSE NULL END) as total_score,
    MAX(CASE WHEN g.grade_type = 'total' THEN g.grade_point ELSE NULL END) as grade_point
FROM users u
JOIN classes c ON u.class_id = c.id
JOIN grades g ON u.id = g.student_id
JOIN courses co ON g.course_id = co.id
WHERE u.role IN ('student', 'class_monitor', 'study_monitor', 'life_monitor', 'sports_monitor')
GROUP BY u.id, u.real_name, c.name, co.id, co.name;

-- 创建存储过程：计算班级平均分
DELIMITER //
CREATE PROCEDURE CalculateClassAverage(IN class_id INT, IN course_id INT)
BEGIN
    SELECT
        c.name as class_name,
        co.name as course_name,
        AVG(g.score) as average_score,
        COUNT(DISTINCT g.student_id) as student_count
    FROM classes c
    JOIN users u ON c.id = u.class_id
    JOIN grades g ON u.id = g.student_id
    JOIN courses co ON g.course_id = co.id
    WHERE c.id = class_id AND co.id = course_id AND g.grade_type = 'total'
    GROUP BY c.id, c.name, co.id, co.name;
END //
DELIMITER ;

-- 创建存储过程：生成班费报表
DELIMITER //
CREATE PROCEDURE GenerateFinanceReport(IN class_id INT, IN start_date DATE, IN end_date DATE)
BEGIN
    SELECT
        fr.record_date,
        fr.type,
        fr.category,
        fr.amount,
        fr.description,
        u.real_name as handler_name
    FROM finance_records fr
    JOIN users u ON fr.handler_id = u.id
    WHERE fr.class_id = class_id
        AND fr.record_date BETWEEN start_date AND end_date
    ORDER BY fr.record_date DESC;
END //
DELIMITER ;