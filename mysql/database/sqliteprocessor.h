#ifndef SQLITEPROCESSOR_H
#define SQLITEPROCESSOR_H

#include <sqlite3.h>

#include "sqlprocessor.h"

class SqliteProcessor final : public SqlProcessor
{
public:
    SqliteProcessor();
    virtual ~SqliteProcessor();

    virtual int InitTable(const std::string& db_file, std::shared_ptr<Table> table) override;
    virtual int GetRowCount(std::shared_ptr<Table> table);
    virtual int InsertData(std::shared_ptr<Table> table, const RowData& row) override;
    virtual int DeleteData(std::shared_ptr<Table> table, const RowData& row) override;
    virtual int UpdateData(std::shared_ptr<Table> table, const RowData& where, const RowData& row) override;
    virtual std::vector<RowData> QueryData(std::shared_ptr<Table> table, const RowData& row) override;
    virtual std::vector<RowData> QueryData(std::shared_ptr<Table> table, const RowData& row, int page_size, int offset);

private:
    std::string create_createsql_cmd(std::shared_ptr<Table> table);
    std::string create_insertsql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields);
    std::string create_deletesql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields);
    std::string create_querysql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields);
    std::string create_querysql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields, int page_size, int offset);
    std::string create_updatesql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields, const std::vector<std::string>& where_fields);

    std::pair<std::vector<std::string>, std::vector<FieldValue>>
    get_field_value(std::shared_ptr<Table> table, const RowData& row);

private:
    sqlite3* handle_;
};

#endif // SQLITEPROCESSOR_H
