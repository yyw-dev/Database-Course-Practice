#ifndef FINANCE_MANAGER_H
#define FINANCE_MANAGER_H

#include "../common.h"
#include "../../database/database_connector.h"

// 财务记录结构
struct FinanceRecord {
    int id = 0;
    int class_id = 0;
    std::string type;           // income, expense
    std::string category;       // 类别：学费、班费、活动经费等
    double amount = 0.0;
    std::string description;
    std::string payer;          // 付款人/收款人
    int handler_id = 0;         // 经手人ID
    std::string record_date;    // 记录日期
    bool is_public = true;      // 是否公开
    TimePoint created_at;

    // 转换为Map便于JSON序列化
    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["class_id"] = std::to_string(class_id);
        map["type"] = type;
        map["category"] = category;
        map["amount"] = std::to_string(amount);
        map["description"] = description;
        map["payer"] = payer;
        map["handler_id"] = std::to_string(handler_id);
        map["record_date"] = record_date;
        map["is_public"] = is_public ? "true" : "false";
        return map;
    }

    static FinanceRecord fromMap(const StringMap& map) {
        FinanceRecord record;
        if (map.count("id")) record.id = std::stoi(map.at("id"));
        if (map.count("class_id")) record.class_id = std::stoi(map.at("class_id"));
        if (map.count("type")) record.type = map.at("type");
        if (map.count("category")) record.category = map.at("category");
        if (map.count("amount")) record.amount = std::stod(map.at("amount"));
        if (map.count("description")) record.description = map.at("description");
        if (map.count("payer")) record.payer = map.at("payer");
        if (map.count("handler_id")) record.handler_id = std::stoi(map.at("handler_id"));
        if (map.count("record_date")) record.record_date = map.at("record_date");
        if (map.count("is_public")) record.is_public = map.at("is_public") == "true";
        return record;
    }
};

// 财务统计结构
struct FinanceStatistics {
    int class_id = 0;
    double total_income = 0.0;
    double total_expense = 0.0;
    double balance = 0.0;
    std::map<std::string, double> income_by_category;
    std::map<std::string, double> expense_by_category;
    std::map<std::string, double> monthly_income;
    std::map<std::string, double> monthly_expense;

    // 转换为Map便于JSON序列化
    StringMap toMap() const {
        StringMap map;
        map["class_id"] = std::to_string(class_id);
        map["total_income"] = std::to_string(total_income);
        map["total_expense"] = std::to_string(total_expense);
        map["balance"] = std::to_string(balance);
        return map;
    }
};

// 预算项目结构
struct BudgetItem {
    int id = 0;
    int class_id = 0;
    std::string category;
    double planned_amount = 0.0;
    double actual_amount = 0.0;
    std::string description;
    std::string semester;
    bool is_approved = false;
    TimePoint created_at;

    StringMap toMap() const {
        StringMap map;
        map["id"] = std::to_string(id);
        map["class_id"] = std::to_string(class_id);
        map["category"] = category;
        map["planned_amount"] = std::to_string(planned_amount);
        map["actual_amount"] = std::to_string(actual_amount);
        map["description"] = description;
        map["semester"] = semester;
        map["is_approved"] = is_approved ? "true" : "false";
        return map;
    }
};

// 财务管理器类
class FinanceManager {
public:
    FinanceManager();
    ~FinanceManager();

    // 财务记录管理
    Result addIncome(const FinanceRecord& record);
    Result addExpense(const FinanceRecord& record);
    Result updateRecord(const FinanceRecord& record);
    Result deleteRecord(int record_id);
    std::optional<FinanceRecord> getRecordById(int record_id);
    std::vector<FinanceRecord> getRecordsByClass(int class_id, const std::string& start_date = "", const std::string& end_date = "");
    std::vector<FinanceRecord> getRecordsByCategory(int class_id, const std::string& category);
    std::vector<FinanceRecord> getRecordsByHandler(int handler_id);

    // 财务查询
    std::vector<FinanceRecord> getPublicRecords(int class_id);
    std::vector<FinanceRecord> getRecordsByType(int class_id, const std::string& type);
    std::vector<FinanceRecord> searchRecords(int class_id, const std::string& keyword);

    // 财务统计
    FinanceStatistics getClassFinanceStatistics(int class_id, const std::string& start_date = "", const std::string& end_date = "");
    double getClassBalance(int class_id);
    std::map<std::string, double> getCategoryStatistics(int class_id, const std::string& type);
    std::vector<FinanceRecord> getTopExpenses(int class_id, int limit = 10);
    std::vector<FinanceRecord> getTopIncomes(int class_id, int limit = 10);

    // 预算管理
    Result createBudgetItem(const BudgetItem& item);
    Result updateBudgetItem(const BudgetItem& item);
    Result approveBudgetItem(int item_id);
    Result deleteBudgetItem(int item_id);
    std::optional<BudgetItem> getBudgetItemById(int item_id);
    std::vector<BudgetItem> getBudgetItemsByClass(int class_id, const std::string& semester = "");
    std::vector<BudgetItem> getPendingBudgetItems(int class_id);

    // 报表生成
    Result generateMonthlyReport(int class_id, const std::string& month);
    Result generateSemesterReport(int class_id, const std::string& semester);

private:
    DatabaseConnector& db_;

    // 内部辅助方法
    bool validateFinanceRecord(const FinanceRecord& record);
    bool validateBudgetItem(const BudgetItem& item);

    // 数据库操作
    Result insertFinanceRecord(const FinanceRecord& record);
    Result updateFinanceRecordInDB(const FinanceRecord& record);
    Result deleteFinanceRecordFromDB(int record_id);

    Result insertBudgetItem(const BudgetItem& item);
    Result updateBudgetItemInDB(const BudgetItem& item);
    Result deleteBudgetItemFromDB(int item_id);

    // 统计计算
    double calculateTotalByType(int class_id, const std::string& type, const std::string& start_date = "", const std::string& end_date = "");
    std::map<std::string, double> calculateCategoryTotals(int class_id, const std::string& type);
    std::map<std::string, double> calculateMonthlyTotals(int class_id, const std::string& type);
};

#endif // FINANCE_MANAGER_H