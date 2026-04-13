#!/bin/bash

# 高校班级事务管理系统运行脚本

set -e

echo "=== 高校班级事务管理系统运行脚本 ==="

# 配置变量
BUILD_DIR="build"
LOG_DIR="logs"
CONFIG_FILE="config/config.json"
DEFAULT_HOST="0.0.0.0"
DEFAULT_PORT="8080"

# 显示帮助信息
show_help() {
    echo "用法: $0 [选项]"
    echo
    echo "选项:"
    echo "  -h, --help           显示此帮助信息"
    echo "  --host HOST          指定服务器主机 (默认: $DEFAULT_HOST)"
    echo "  --port PORT          指定服务器端口 (默认: $DEFAULT_PORT)"
    echo "  --config FILE        指定配置文件路径 (默认: $CONFIG_FILE)"
    echo "  --log-level LEVEL    设置日志级别 (debug, info, warning, error, fatal)"
    echo "  --daemon             后台运行"
    echo "  --stop               停止后台运行的实例"
    echo "  --status             显示运行状态"
    echo "  --restart            重启服务"
    echo
    echo "示例:"
    echo "  $0 --host 127.0.0.1 --port 8080"
    echo "  $0 --daemon"
    echo "  $0 --stop"
}

# 检查构建目录和可执行文件
check_executable() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "错误: 构建目录不存在，请先运行构建脚本"
        echo "运行: ./scripts/build.sh"
        exit 1
    fi

    if [ ! -f "$BUILD_DIR/class_management_system" ]; then
        echo "错误: 可执行文件不存在，请先构建项目"
        echo "运行: ./scripts/build.sh"
        exit 1
    fi
}

# 创建必要的目录
create_directories() {
    mkdir -p $LOG_DIR
    mkdir -p config
}

# 检查数据库连接
check_database() {
    echo "检查数据库连接..."

    # 简单的数据库连接检查
    if command -v mysql &> /dev/null; then
        if mysql -u class_app -p -e "USE class_management_system;" >/dev/null 2>&1; then
            echo "✓ 数据库连接正常"
        else
            echo "警告: 数据库连接失败，请检查数据库配置"
        fi
    else
        echo "警告: mysql客户端未安装，跳过数据库连接检查"
    fi
}

# 检查端口是否被占用
check_port() {
    local port=$1
    if command -v netstat &> /dev/null; then
        if netstat -tlnp | grep -q ":$port\b"; then
            echo "错误: 端口 $port 已被占用"
            exit 1
        fi
    elif command -v ss &> /dev/null; then
        if ss -tlnp | grep -q ":$port\b"; then
            echo "错误: 端口 $port 已被占用"
            exit 1
        fi
    fi
}

# 前台运行
run_foreground() {
    local host=$1
    local port=$2
    local log_level=$3

    echo "前台运行系统..."
    echo "主机: $host"
    echo "端口: $port"
    echo "日志级别: $log_level"
    echo "日志文件: $LOG_DIR/class_management_system.log"
    echo
    echo "按 Ctrl+C 停止服务"

    # 检查端口
    check_port $port

    # 运行程序
    ./$BUILD_DIR/class_management_system \
        --host $host \
        --port $port \
        --log-level $log_level
}

# 后台运行
run_daemon() {
    local host=$1
    local port=$2
    local log_level=$3

    echo "后台运行系统..."

    # 检查是否已经在运行
    if [ -f "$LOG_DIR/system.pid" ]; then
        local pid=$(cat "$LOG_DIR/system.pid")
        if kill -0 $pid >/dev/null 2>&1; then
            echo "错误: 系统已经在运行 (PID: $pid)"
            exit 1
        else
            rm -f "$LOG_DIR/system.pid"
        fi
    fi

    # 检查端口
    check_port $port

    # 后台运行
    nohup ./$BUILD_DIR/class_management_system \
        --host $host \
        --port $port \
        --log-level $log_level \
        > "$LOG_DIR/nohup.out" 2>&1 &

    local pid=$!
    echo $pid > "$LOG_DIR/system.pid"

    echo "系统已后台运行 (PID: $pid)"
    echo "日志文件: $LOG_DIR/class_management_system.log"
    echo "停止服务: ./scripts/run.sh --stop"
}

# 停止服务
stop_service() {
    if [ -f "$LOG_DIR/system.pid" ]; then
        local pid=$(cat "$LOG_DIR/system.pid")
        if kill -0 $pid >/dev/null 2>&1; then
            echo "停止服务 (PID: $pid)..."
            kill $pid
            sleep 2

            # 强制停止
            if kill -0 $pid >/dev/null 2>&1; then
                echo "强制停止服务..."
                kill -9 $pid
            fi

            rm -f "$LOG_DIR/system.pid"
            echo "服务已停止"
        else
            echo "服务未运行"
            rm -f "$LOG_DIR/system.pid"
        fi
    else
        echo "服务未运行"
    fi
}

# 显示运行状态
show_status() {
    if [ -f "$LOG_DIR/system.pid" ]; then
        local pid=$(cat "$LOG_DIR/system.pid")
        if kill -0 $pid >/dev/null 2>&1; then
            echo "服务正在运行 (PID: $pid)"
            echo "运行时间: $(ps -p $pid -o etime=)"
            echo "内存使用: $(ps -p $pid -o rss=) KB"
        else
            echo "服务未运行 (PID文件存在但进程不存在)"
        fi
    else
        echo "服务未运行"
    fi
}

# 重启服务
restart_service() {
    echo "重启服务..."
    stop_service
    sleep 2
    run_daemon $DEFAULT_HOST $DEFAULT_PORT "info"
}

# 解析命令行参数
HOST=$DEFAULT_HOST
PORT=$DEFAULT_PORT
LOG_LEVEL="info"
DAEMON=false
STOP=false
STATUS=false
RESTART=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        --host)
            HOST="$2"
            shift 2
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --log-level)
            LOG_LEVEL="$2"
            shift 2
            ;;
        --daemon)
            DAEMON=true
            shift
            ;;
        --stop)
            STOP=true
            shift
            ;;
        --status)
            STATUS=true
            shift
            ;;
        --restart)
            RESTART=true
            shift
            ;;
        *)
            echo "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 执行命令
if [ "$STOP" = true ]; then
    stop_service
elif [ "$STATUS" = true ]; then
    show_status
elif [ "$RESTART" = true ]; then
    restart_service
else
    # 检查可执行文件
    check_executable

    # 创建目录
    create_directories

    # 检查数据库
    check_database

    # 运行服务
    if [ "$DAEMON" = true ]; then
        run_daemon $HOST $PORT $LOG_LEVEL
    else
        run_foreground $HOST $PORT $LOG_LEVEL
    fi
fi

echo "=== 操作完成 ==="