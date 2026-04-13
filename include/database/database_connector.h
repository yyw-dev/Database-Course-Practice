#ifndef DATABASE_CONNECTOR_H
#define DATABASE_CONNECTOR_H

#include "../common.h"
#include <mysql/mysql.h>
#include <mutex>

class DatabaseConnector {
public:
    static DatabaseConnector& getInstance();

    // 连接管理
    bool connect(const std::string& host, int port,
                const std::string& user, const std::string& password,
                const std::string& database);
    void disconnect();
    bool isConnected() const;

    // 查询执行
    bool execute(const std::string& query);
    bool executeQuery(const std::string& query, MYSQL_RES** result);

    // 事务管理
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // 预处理语句
    MYSQL_STMT* prepareStatement(const std::string& query);
    bool executeStatement(MYSQL_STMT* stmt);

    // 错误处理
    std::string getLastError() const;
    unsigned int getLastErrorNumber() const;

    // 获取MySQL连接（用于获取最后插入的ID等）
    MYSQL* getConnection() const { return connection_; }

private:
    DatabaseConnector();
    ~DatabaseConnector();
    DatabaseConnector(const DatabaseConnector&) = delete;
    DatabaseConnector& operator=(const DatabaseConnector&) = delete;

    MYSQL* connection_;
    bool connected_;
    mutable std::mutex mutex_;
};

// 数据库结果集包装类
class DatabaseResult {
public:
    DatabaseResult(MYSQL_RES* result);
    ~DatabaseResult();

    bool next();
    std::string getString(const std::string& field) const;
    int getInt(const std::string& field) const;
    double getDouble(const std::string& field) const;
    long long getLongLong(const std::string& field) const;
    bool getBool(const std::string& field) const;

    int getColumnCount() const;
    std::string getColumnName(int index) const;
    bool isNull(const std::string& field) const;

    // 获取所有行
    std::vector<StringMap> fetchAll();

private:
    MYSQL_RES* result_;
    MYSQL_ROW row_;
    std::vector<std::string> fieldNames_;
};

#endif // DATABASE_CONNECTOR_H