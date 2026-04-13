#!/bin/bash

# 高校班级事务管理系统数据库设置脚本

set -e

echo "=== 高校班级事务管理系统数据库设置 ==="

# 配置变量
DB_HOST="localhost"
DB_PORT="3306"
DB_ROOT_USER="root"
DB_APP_USER="class_app"
DB_APP_PASSWORD=""
DB_NAME="class_management_system"

echo "请输入数据库root密码:"
read -s DB_ROOT_PASSWORD
echo
e
echo "请输入应用数据库用户密码 (直接回车使用默认密码):"
read DB_APP_PASSWORD

if [ -z "$DB_APP_PASSWORD" ]; then
    DB_APP_PASSWORD="ClassApp2024!"
fi

# 测试数据库连接
echo "测试数据库连接..."
if ! mysql -h $DB_HOST -P $DB_PORT -u $DB_ROOT_USER -p$DB_ROOT_PASSWORD -e "SELECT 1" >/dev/null 2>&1; then
    echo "错误: 无法连接到数据库，请检查root密码是否正确"
    exit 1
fi

echo "数据库连接成功"

# 创建数据库
echo "创建数据库 $DB_NAME..."
mysql -h $DB_HOST -P $DB_PORT -u $DB_ROOT_USER -p$DB_ROOT_PASSWORD <<EOF
CREATE DATABASE IF NOT EXISTS $DB_NAME CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
EOF

# 创建应用用户
echo "创建应用用户 $DB_APP_USER..."
mysql -h $DB_HOST -P $DB_PORT -u $DB_ROOT_USER -p$DB_ROOT_PASSWORD <<EOF
CREATE USER IF NOT EXISTS '$DB_APP_USER'@'localhost' IDENTIFIED BY '$DB_APP_PASSWORD';
CREATE USER IF NOT EXISTS '$DB_APP_USER'@'%' IDENTIFIED BY '$DB_APP_PASSWORD';
GRANT ALL PRIVILEGES ON $DB_NAME.* TO '$DB_APP_USER'@'localhost';
GRANT ALL PRIVILEGES ON $DB_NAME.* TO '$DB_APP_USER'@'%';
FLUSH PRIVILEGES;
EOF

# 导入数据库结构
echo "导入数据库结构..."
if [ -f "database/schema.sql" ]; then
    mysql -h $DB_HOST -P $DB_PORT -u $DB_APP_USER -p$DB_APP_PASSWORD $DB_NAME < database/schema.sql
    echo "数据库结构导入成功"
else
    echo "错误: 找不到数据库结构文件 database/schema.sql"
    exit 1
fi

# 导入初始数据
echo "导入初始数据..."
if [ -f "database/init_data.sql" ]; then
    mysql -h $DB_HOST -P $DB_PORT -u $DB_APP_USER -p$DB_APP_PASSWORD $DB_NAME < database/init_data.sql
    echo "初始数据导入成功"
else
    echo "警告: 找不到初始数据文件 database/init_data.sql"
fi

echo "=== 数据库设置完成 ==="
echo "数据库名称: $DB_NAME"
echo "应用用户: $DB_APP_USER"
echo "应用密码: $DB_APP_PASSWORD"
echo "请妥善保存上述信息"
echo
echo "默认管理员账户:"
echo "用户名: admin"
echo "密码: admin123"
echo "请首次登录后立即修改密码"