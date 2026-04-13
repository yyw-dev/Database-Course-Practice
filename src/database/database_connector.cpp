#include "../../include/database/database_connector.h"
#include "../../include/utils/logger.h"
#include <stdexcept>
#include <iostream>

DatabaseConnector::DatabaseConnector() : connection_(nullptr), connected_(false) {
    connection_ = mysql_init(nullptr);
    if (!connection_) {
        throw std::runtime_error("Failed to initialize MySQL connection");
    }
}

DatabaseConnector::~DatabaseConnector() {
    disconnect();
    if (connection_) {
        mysql_close(connection_);
    }
}

DatabaseConnector& DatabaseConnector::getInstance() {
    static DatabaseConnector instance;
    return instance;
}

bool DatabaseConnector::connect(const std::string& host, int port,
                              const std::string& user, const std::string& password,
                              const std::string& database) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (connected_) {
        disconnect();
    }

    mysql_options(connection_, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    mysql_options(connection_, MYSQL_INIT_COMMAND, "SET NAMES utf8mb4");

    if (!mysql_real_connect(connection_, host.c_str(), user.c_str(),
                           password.c_str(), database.c_str(), port, nullptr, 0)) {
        LOG_ERROR("Database connection failed: " + std::string(mysql_error(connection_)));
        return false;
    }

    connected_ = true;
    LOG_INFO("Database connected successfully to " + host + ":" + std::to_string(port));
    return true;
}

void DatabaseConnector::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (connected_ && connection_) {
        mysql_close(connection_);
        connected_ = false;
        LOG_INFO("Database connection closed");
    }
}

bool DatabaseConnector::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_ && mysql_ping(connection_) == 0;
}

bool DatabaseConnector::execute(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_) {
        LOG_ERROR("Database not connected");
        return false;
    }

    if (mysql_query(connection_, query.c_str())) {
        LOG_ERROR("Query execution failed: " + std::string(mysql_error(connection_)));
        LOG_ERROR("Failed query: " + query);
        return false;
    }

    return true;
}

bool DatabaseConnector::executeQuery(const std::string& query, MYSQL_RES** result) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_) {
        LOG_ERROR("Database not connected");
        return false;
    }

    if (mysql_query(connection_, query.c_str())) {
        LOG_ERROR("Query execution failed: " + std::string(mysql_error(connection_)));
        LOG_ERROR("Failed query: " + query);
        return false;
    }

    *result = mysql_store_result(connection_);
    return *result != nullptr;
}

bool DatabaseConnector::beginTransaction() {
    return execute("START TRANSACTION");
}

bool DatabaseConnector::commitTransaction() {
    return execute("COMMIT");
}

bool DatabaseConnector::rollbackTransaction() {
    return execute("ROLLBACK");
}

MYSQL_STMT* DatabaseConnector::prepareStatement(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!connected_) {
        LOG_ERROR("Database not connected");
        return nullptr;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(connection_);
    if (!stmt) {
        LOG_ERROR("Failed to initialize statement");
        return nullptr;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        LOG_ERROR("Failed to prepare statement: " + std::string(mysql_stmt_error(stmt)));
        mysql_stmt_close(stmt);
        return nullptr;
    }

    return stmt;
}

bool DatabaseConnector::executeStatement(MYSQL_STMT* stmt) {
    if (!stmt) {
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        LOG_ERROR("Failed to execute statement: " + std::string(mysql_stmt_error(stmt)));
        return false;
    }

    return true;
}

std::string DatabaseConnector::getLastError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (connection_) {
        return mysql_error(connection_);
    }
    return "No database connection";
}

unsigned int DatabaseConnector::getLastErrorNumber() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (connection_) {
        return mysql_errno(connection_);
    }
    return 0;
}

// DatabaseResult 实现
DatabaseResult::DatabaseResult(MYSQL_RES* result) : result_(result), row_(nullptr) {
    if (result_) {
        // 获取字段名
        int num_fields = mysql_num_fields(result_);
        MYSQL_FIELD* fields = mysql_fetch_fields(result_);
        for (int i = 0; i < num_fields; i++) {
            fieldNames_.push_back(fields[i].name);
        }
    }
}

DatabaseResult::~DatabaseResult() {
    if (result_) {
        mysql_free_result(result_);
    }
}

bool DatabaseResult::next() {
    if (!result_) {
        return false;
    }
    row_ = mysql_fetch_row(result_);
    return row_ != nullptr;
}

std::string DatabaseResult::getString(const std::string& field) const {
    if (!row_ || !result_) {
        return "";
    }

    int num_fields = mysql_num_fields(result_);
    MYSQL_FIELD* fields = mysql_fetch_fields(result_);

    for (int i = 0; i < num_fields; i++) {
        if (fieldNames_[i] == field) {
            return row_[i] ? std::string(row_[i]) : "";
        }
    }

    return "";
}

int DatabaseResult::getInt(const std::string& field) const {
    std::string value = getString(field);
    return value.empty() ? 0 : std::stoi(value);
}

double DatabaseResult::getDouble(const std::string& field) const {
    std::string value = getString(field);
    return value.empty() ? 0.0 : std::stod(value);
}

long long DatabaseResult::getLongLong(const std::string& field) const {
    std::string value = getString(field);
    return value.empty() ? 0 : std::stoll(value);
}

bool DatabaseResult::getBool(const std::string& field) const {
    std::string value = getString(field);
    return !value.empty() && (value == "1" || value == "true");
}

int DatabaseResult::getColumnCount() const {
    return result_ ? mysql_num_fields(result_) : 0;
}

std::string DatabaseResult::getColumnName(int index) const {
    if (!result_ || index < 0 || index >= getColumnCount()) {
        return "";
    }
    return fieldNames_[index];
}

bool DatabaseResult::isNull(const std::string& field) const {
    if (!row_ || !result_) {
        return true;
    }

    int num_fields = mysql_num_fields(result_);
    for (int i = 0; i < num_fields; i++) {
        if (fieldNames_[i] == field) {
            return row_[i] == nullptr;
        }
    }

    return true;
}

std::vector<StringMap> DatabaseResult::fetchAll() {
    std::vector<StringMap> results;

    while (next()) {
        StringMap row;
        for (size_t i = 0; i < fieldNames_.size(); i++) {
            row[fieldNames_[i]] = row_[i] ? std::string(row_[i]) : "";
        }
        results.push_back(row);
    }

    // 重置结果集指针到开始位置
    mysql_data_seek(result_, 0);

    return results;
}