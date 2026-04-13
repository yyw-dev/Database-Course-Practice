#include "../../include/core/finance/finance_manager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/validator.h"
#include <sstream>
#include <iomanip>

FinanceManager::FinanceManager() : db_(DatabaseConnector::getInstance()) {
    LOG_INFO("FinanceManager initialized");
}

FinanceManager::~FinanceManager() {
    LOG_INFO("FinanceManager destroyed");
}

Result FinanceManager::addIncome(const FinanceRecord& record) {
    if (record.type != "income") {
        FinanceRecord correctedRecord = record;
        correctedRecord.type = "income";
        return insertFinanceRecord(correctedRecord);
    }
    return insertFinanceRecord(record);
}

Result FinanceManager::addExpense(const FinanceRecord& record) {
    if (record.type != "expense") {
        FinanceRecord correctedRecord = record;
        correctedRecord.type = "expense";
        return insertFinanceRecord(correctedRecord);
    }
    return insertFinanceRecord(record);
}

Result FinanceManager::updateRecord(const FinanceRecord& record) {
    if (!validateFinanceRecord(record)) {
        return Result::Failure("财务记录数据验证失败");
    }

    auto existingRecord = getRecordById(record.id);
    if (!existingRecord) {
        return Result::Failure("财务记录不存在");
    }

    return updateFinanceRecordInDB(record);
}

Result FinanceManager::deleteRecord(int record_id) {
    auto record = getRecordById(record_id);
    if (!record) {
        return Result::Failure("财务记录不存在");
    }

    return deleteFinanceRecordFromDB(record_id);
}

std::optional<FinanceRecord> FinanceManager::getRecordById(int record_id) {
    std::stringstream query;
    query << "SELECT fr.*, u.real_name as handler_name FROM finance_records fr "
          << "LEFT JOIN users u ON fr.handler_id = u.id "
          << "WHERE fr.id = " << record_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            return record;
        }
    }

    return std::nullopt;
}

std::vector<FinanceRecord> FinanceManager::getRecordsByClass(int class_id, const std::string& start_date, const std::string& end_date) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id;

    if (!start_date.empty()) {
        query << " AND record_date >= '" << start_date << "'";
    }
    if (!end_date.empty()) {
        query << " AND record_date <= '" << end_date << "'";
    }

    query << " ORDER BY record_date DESC, created_at DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

std::vector<FinanceRecord> FinanceManager::getRecordsByCategory(int class_id, const std::string& category) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id
          << " AND category = '" << Validator::sanitizeString(category) << "'"
          << " ORDER BY record_date DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

std::vector<FinanceRecord> FinanceManager::getRecordsByHandler(int handler_id) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE handler_id = " << handler_id
          << " ORDER BY record_date DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

std::vector<FinanceRecord> FinanceManager::getPublicRecords(int class_id) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id
          << " AND is_public = TRUE ORDER BY record_date DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

std::vector<FinanceRecord> FinanceManager::getRecordsByType(int class_id, const std::string& type) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id
          << " AND type = '" << type << "' ORDER BY record_date DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

std::vector<FinanceRecord> FinanceManager::searchRecords(int class_id, const std::string& keyword) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id
          << " AND (description LIKE '%" << Validator::sanitizeString(keyword) << "%'"
          << " OR category LIKE '%" << Validator::sanitizeString(keyword) << "%'"
          << " OR payer LIKE '%" << Validator::sanitizeString(keyword) << "%')"
          << " ORDER BY record_date DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

FinanceStatistics FinanceManager::getClassFinanceStatistics(int class_id, const std::string& start_date, const std::string& end_date) {
    FinanceStatistics stats;
    stats.class_id = class_id;

    stats.total_income = calculateTotalByType(class_id, "income", start_date, end_date);
    stats.total_expense = calculateTotalByType(class_id, "expense", start_date, end_date);
    stats.balance = stats.total_income - stats.total_expense;

    stats.income_by_category = calculateCategoryTotals(class_id, "income");
    stats.expense_by_category = calculateCategoryTotals(class_id, "expense");

    stats.monthly_income = calculateMonthlyTotals(class_id, "income");
    stats.monthly_expense = calculateMonthlyTotals(class_id, "expense");

    return stats;
}

double FinanceManager::getClassBalance(int class_id) {
    return getClassFinanceStatistics(class_id).balance;
}

std::map<std::string, double> FinanceManager::getCategoryStatistics(int class_id, const std::string& type) {
    return calculateCategoryTotals(class_id, type);
}

std::vector<FinanceRecord> FinanceManager::getTopExpenses(int class_id, int limit) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id
          << " AND type = 'expense' ORDER BY amount DESC LIMIT " << limit;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

std::vector<FinanceRecord> FinanceManager::getTopIncomes(int class_id, int limit) {
    std::vector<FinanceRecord> records;

    std::stringstream query;
    query << "SELECT * FROM finance_records WHERE class_id = " << class_id
          << " AND type = 'income' ORDER BY amount DESC LIMIT " << limit;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            FinanceRecord record;
            record.id = dbResult.getInt("id");
            record.class_id = dbResult.getInt("class_id");
            record.type = dbResult.getString("type");
            record.category = dbResult.getString("category");
            record.amount = dbResult.getDouble("amount");
            record.description = dbResult.getString("description");
            record.payer = dbResult.getString("payer");
            record.handler_id = dbResult.getInt("handler_id");
            record.record_date = dbResult.getString("record_date");
            record.is_public = dbResult.getBool("is_public");

            records.push_back(record);
        }
    }

    return records;
}

Result FinanceManager::createBudgetItem(const BudgetItem& item) {
    if (!validateBudgetItem(item)) {
        return Result::Failure("预算项目数据验证失败");
    }

    return insertBudgetItem(item);
}

Result FinanceManager::updateBudgetItem(const BudgetItem& item) {
    if (!validateBudgetItem(item)) {
        return Result::Failure("预算项目数据验证失败");
    }

    auto existingItem = getBudgetItemById(item.id);
    if (!existingItem) {
        return Result::Failure("预算项目不存在");
    }

    return updateBudgetItemInDB(item);
}

Result FinanceManager::approveBudgetItem(int item_id) {
    auto item = getBudgetItemById(item_id);
    if (!item) {
        return Result::Failure("预算项目不存在");
    }

    std::stringstream query;
    query << "UPDATE budget_items SET is_approved = TRUE WHERE id = " << item_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Budget item approved: ID %d", item_id);
        return Result::Success("预算项目审批通过");
    } else {
        LOG_ERROR_FMT("Failed to approve budget item: ID %d", item_id);
        return Result::Failure("预算项目审批失败: " + db_.getLastError());
    }
}

Result FinanceManager::deleteBudgetItem(int item_id) {
    auto item = getBudgetItemById(item_id);
    if (!item) {
        return Result::Failure("预算项目不存在");
    }

    return deleteBudgetItemFromDB(item_id);
}

std::optional<BudgetItem> FinanceManager::getBudgetItemById(int item_id) {
    std::stringstream query;
    query << "SELECT * FROM budget_items WHERE id = " << item_id;

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        if (dbResult.next()) {
            BudgetItem item;
            item.id = dbResult.getInt("id");
            item.class_id = dbResult.getInt("class_id");
            item.category = dbResult.getString("category");
            item.planned_amount = dbResult.getDouble("planned_amount");
            item.actual_amount = dbResult.getDouble("actual_amount");
            item.description = dbResult.getString("description");
            item.semester = dbResult.getString("semester");
            item.is_approved = dbResult.getBool("is_approved");

            return item;
        }
    }

    return std::nullopt;
}

std::vector<BudgetItem> FinanceManager::getBudgetItemsByClass(int class_id, const std::string& semester) {
    std::vector<BudgetItem> items;

    std::stringstream query;
    query << "SELECT * FROM budget_items WHERE class_id = " << class_id;

    if (!semester.empty()) {
        query << " AND semester = '" << Validator::sanitizeString(semester) << "'";
    }

    query << " ORDER BY created_at DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            BudgetItem item;
            item.id = dbResult.getInt("id");
            item.class_id = dbResult.getInt("class_id");
            item.category = dbResult.getString("category");
            item.planned_amount = dbResult.getDouble("planned_amount");
            item.actual_amount = dbResult.getDouble("actual_amount");
            item.description = dbResult.getString("description");
            item.semester = dbResult.getString("semester");
            item.is_approved = dbResult.getBool("is_approved");

            items.push_back(item);
        }
    }

    return items;
}

std::vector<BudgetItem> FinanceManager::getPendingBudgetItems(int class_id) {
    std::vector<BudgetItem> items;

    std::stringstream query;
    query << "SELECT * FROM budget_items WHERE class_id = " << class_id
          << " AND is_approved = FALSE ORDER BY created_at DESC";

    MYSQL_RES* result;
    if (db_.executeQuery(query.str(), &result)) {
        DatabaseResult dbResult(result);
        while (dbResult.next()) {
            BudgetItem item;
            item.id = dbResult.getInt("id");
            item.class_id = dbResult.getInt("class_id");
            item.category = dbResult.getString("category");
            item.planned_amount = dbResult.getDouble("planned_amount");
            item.actual_amount = dbResult.getDouble("actual_amount");
            item.description = dbResult.getString("description");
            item.semester = dbResult.getString("semester");
            item.is_approved = dbResult.getBool("is_approved");

            items.push_back(item);
        }
    }

    return items;
}

Result FinanceManager::generateMonthlyReport(int class_id, const std::string& month) {
    // 这里可以实现生成月度报表的逻辑
    // 实际项目中可能需要生成PDF或Excel文件
    LOG_INFO_FMT("Generating monthly report for class %d, month: %s", class_id, month.c_str());
    return Result::Success("月度报表生成成功");
}

Result FinanceManager::generateSemesterReport(int class_id, const std::string& semester) {
    // 这里可以实现生成学期报表的逻辑
    LOG_INFO_FMT("Generating semester report for class %d, semester: %s", class_id, semester.c_str());
    return Result::Success("学期报表生成成功");
}

// 私有方法实现
bool FinanceManager::validateFinanceRecord(const FinanceRecord& record) {
    if (record.class_id <= 0 || record.amount <= 0 || record.category.empty()) {
        return false;
    }

    if (record.type != "income" && record.type != "expense") {
        return false;
    }

    if (record.description.empty() || record.description.length() > 500) {
        return false;
    }

    if (!Validator::validateDate(record.record_date)) {
        return false;
    }

    return true;
}

bool FinanceManager::validateBudgetItem(const BudgetItem& item) {
    if (item.class_id <= 0 || item.category.empty() || item.semester.empty()) {
        return false;
    }

    if (item.planned_amount < 0) {
        return false;
    }

    if (item.description.length() > 500) {
        return false;
    }

    return true;
}

Result FinanceManager::insertFinanceRecord(const FinanceRecord& record) {
    if (!validateFinanceRecord(record)) {
        return Result::Failure("财务记录数据验证失败");
    }

    std::stringstream query;
    query << "INSERT INTO finance_records (class_id, type, category, amount, description, "
          << "payer, handler_id, record_date, is_public) VALUES ("
          << record.class_id << ", "
          << "'" << record.type << "', "
          << "'" << Validator::sanitizeString(record.category) << "', "
          << std::fixed << std::setprecision(2) << record.amount << ", "
          << "'" << Validator::sanitizeString(record.description) << "', "
          << "'" << Validator::sanitizeString(record.payer) << "', "
          << record.handler_id << ", "
          << "'" << record.record_date << "', "
          << (record.is_public ? "TRUE" : "FALSE")
          << ")";

    if (db_.execute(query.str())) {
        int record_id = mysql_insert_id(db_.getConnection());
        LOG_INFO_FMT("Finance record created: ID %d, type: %s, amount: %.2f",
                    record_id, record.type.c_str(), record.amount);
        return Result::Success("财务记录添加成功");
    } else {
        LOG_ERROR_FMT("Failed to create finance record: %s", db_.getLastError().c_str());
        return Result::Failure("财务记录添加失败: " + db_.getLastError());
    }
}

Result FinanceManager::updateFinanceRecordInDB(const FinanceRecord& record) {
    std::stringstream query;
    query << "UPDATE finance_records SET "
          << "category = '" << Validator::sanitizeString(record.category) << "', "
          << "amount = " << std::fixed << std::setprecision(2) << record.amount << ", "
          << "description = '" << Validator::sanitizeString(record.description) << "', "
          << "payer = '" << Validator::sanitizeString(record.payer) << "', "
          << "handler_id = " << record.handler_id << ", "
          << "record_date = '" << record.record_date << "', "
          << "is_public = " << (record.is_public ? "TRUE" : "FALSE")
          << " WHERE id = " << record.id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Finance record updated: ID %d", record.id);
        return Result::Success("财务记录更新成功");
    } else {
        LOG_ERROR_FMT("Failed to update finance record: ID %d", record.id);
        return Result::Failure("财务记录更新失败: " + db_.getLastError());
    }
}

Result FinanceManager::deleteFinanceRecordFromDB(int record_id) {
    std::stringstream query;
    query << "DELETE FROM finance_records WHERE id = " << record_id;

    if (db_.execute(query.str())) {
        LOG_INFO_FMT("Finance record deleted: ID %d", record_id);
        return Result::Success("财务记录删除成功");
    } else {
        LOG_ERROR_FMT("Failed to delete finance record: ID %d", record_id);
        return Result::Failure("财务记录删除失败: " + db_.getLastError());
    }
}

Result FinanceManager::insertBudgetItem(const BudgetItem& item) {
    std::stringstream query;
    query << "INSERT INTO budget_items (class_id, category, planned_amount, description, semester) VALUES ("
          << item.class_id << ", "
          << "'" << Validator::sanitizeString(item.category) << "', "
          << std::fixed << std::setprecision(2) << item.planned_amount << ", "
          << "'" << Validator::sanitizeString(item.description) << "', "
          << "'" << Validator::sanitizeString(item.semester) << "')";

    if (db_.execute(query.str())) {
        int item_id = mysql_insert_id(db_.getConnection());
        LOG_INFO_FMT("Budget item created: ID %d, category: %s", item_id, item.category.c_str());
        return Result::Success("预算项目创建成功");
