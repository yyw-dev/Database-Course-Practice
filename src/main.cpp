#include "../include/common.h"
#include "../include/database/database_connector.h"
#include "../include/utils/logger.h"
#include "../include/utils/config_loader.h"
#include "../include/web/server.h"
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>

// 全局变量
std::unique_ptr<WebServer> g_server;
bool g_running = true;

// 信号处理函数
void signalHandler(int signal) {
    LOG_INFO_FMT("Received signal %d, shutting down gracefully...", signal);
    g_running = false;

    if (g_server) {
        g_server->stop();
    }
}

// 数据库初始化
bool initializeDatabase() {
    try {
        auto& db = DatabaseConnector::getInstance();

        // 从配置文件读取数据库连接信息
        std::string host = "localhost";
        int port = 3306;
        std::string user = "class_app";
        std::string password = "password";
        std::string database = "class_management_system";

        // 尝试连接数据库
        if (!db.connect(host, port, user, password, database)) {
            LOG_ERROR("Failed to connect to database");
            return false;
        }

        LOG_INFO("Database connected successfully");
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Database initialization error: %s", e.what());
        return false;
    }
}

// 应用程序初始化
bool initializeApplication() {
    try {
        // 初始化日志系统
        Logger::getInstance().setLogLevel(LogLevel::INFO);
        Logger::getInstance().enableConsoleOutput(true);
        Logger::getInstance().setLogFile("class_management_system.log");

        LOG_INFO("=== 高校班级事务管理系统启动 ===");
        LOG_INFO("系统版本: 1.0.0");
        LOG_INFO("编译时间: " __DATE__ " " __TIME__);

        // 初始化数据库
        if (!initializeDatabase()) {
            LOG_ERROR("Database initialization failed");
            return false;
        }

        // 初始化Web服务器
        g_server = std::make_unique<WebServer>();

        std::string host = "0.0.0.0";
        int port = 8080;

        if (!g_server->initialize(host, port)) {
            LOG_ERROR("Web server initialization failed");
            return false;
        }

        LOG_INFO_FMT("Application initialized successfully on %s:%d", host.c_str(), port);
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Application initialization error: %s", e.what());
        return false;
    }
}

// 显示系统信息
void showSystemInfo() {
    LOG_INFO("=== 系统信息 ===");
    LOG_INFO("操作系统: Linux");
    LOG_INFO("编译器: GCC " __VERSION__);
    LOG_INFO("C++标准: C++17");
    LOG_INFO("构建类型: " CMAKE_BUILD_TYPE);
    LOG_INFO("==================");
}

// 显示帮助信息
void showHelp() {
    std::cout << "高校班级事务管理系统 v1.0.0" << std::endl;
    std::cout << "用法: class_management_system [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help           显示此帮助信息" << std::endl;
    std::cout << "  -v, --version        显示版本信息" << std::endl;
    std::cout << "  --host HOST          指定服务器主机 (默认: 0.0.0.0)" << std::endl;
    std::cout << "  --port PORT          指定服务器端口 (默认: 8080)" << std::endl;
    std::cout << "  --log-level LEVEL    设置日志级别 (debug, info, warning, error, fatal)" << std::endl;
    std::cout << "  --config FILE        指定配置文件路径" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  ./class_management_system --host 127.0.0.1 --port 8080" << std::endl;
    std::cout << "  ./class_management_system --config /etc/class_management.conf" << std::endl;
}

// 显示版本信息
void showVersion() {
    std::cout << "高校班级事务管理系统 v1.0.0" << std::endl;
    std::cout << "基于C++17开发" << std::endl;
    std::cout << "编译时间: " << __DATE__ << " " << __TIME__ << std::endl;
}

// 解析命令行参数
bool parseCommandLine(int argc, char* argv[], std::string& host, int& port, std::string& logLevel) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            showHelp();
            return false;
        } else if (arg == "-v" || arg == "--version") {
            showVersion();
            return false;
        } else if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--log-level" && i + 1 < argc) {
            logLevel = argv[++i];
        } else {
            std::cerr << "未知参数: " << arg << std::endl;
            showHelp();
            return false;
        }
    }

    return true;
}

// 主循环
void mainLoop() {
    LOG_INFO("系统启动完成，进入主循环");

    while (g_running) {
        try {
            // 检查数据库连接状态
            auto& db = DatabaseConnector::getInstance();
            if (!db.isConnected()) {
                LOG_ERROR("数据库连接断开，尝试重新连接...");
                if (!initializeDatabase()) {
                    LOG_ERROR("数据库重新连接失败，等待5秒后重试...");
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    continue;
                }
            }

            // 检查服务器状态
            if (g_server && !g_server->isRunning()) {
                LOG_WARNING("Web服务器未运行，尝试重新启动...");
                g_server->start();
            }

            // 休眠1秒
            std::this_thread::sleep_for(std::chrono::seconds(1));

        } catch (const std::exception& e) {
            LOG_ERROR_FMT("主循环异常: %s", e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

// 清理资源
void cleanup() {
    LOG_INFO("正在清理资源...");

    if (g_server) {
        g_server->stop();
        g_server.reset();
    }

    // 关闭数据库连接
    try {
        DatabaseConnector::getInstance().disconnect();
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("数据库断开连接时发生错误: %s", e.what());
    }

    LOG_INFO("资源清理完成");
}

int main(int argc, char* argv[]) {
    // 设置信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 解析命令行参数
    std::string host = "0.0.0.0";
    int port = 8080;
    std::string logLevel = "info";

    if (!parseCommandLine(argc, argv, host, port, logLevel)) {
        return 0;
    }

    try {
        // 显示系统信息
        showSystemInfo();

        // 初始化应用程序
        if (!initializeApplication()) {
            LOG_FATAL("应用程序初始化失败");
            return 1;
        }

        // 启动Web服务器
        g_server->start();

        // 进入主循环
        mainLoop();

        LOG_INFO("系统正常关闭");
        return 0;

    } catch (const std::exception& e) {
        LOG_FATAL_FMT("系统发生致命错误: %s", e.what());
        cleanup();
        return 1;
    } catch (...) {
        LOG_FATAL("系统发生未知致命错误");
        cleanup();
        return 1;
    }

    cleanup();
    return 0;
}