#ifndef SQLPROCESSOR_H
#define SQLPROCESSOR_H

#include <string>
#include <memory>

#include "table.h"

class SqlProcessor {
public:
    using RowData = std::map<std::string, FieldValue>;

public:
    SqlProcessor() {}
    virtual ~SqlProcessor() {}

    virtual int InitTable(const std::string& db_file, std::shared_ptr<Table> table) = 0;
    virtual int GetRowCount(std::shared_ptr<Table> table) = 0;
    virtual int InsertData(std::shared_ptr<Table> table, const RowData& row) = 0;
    virtual int DeleteData(std::shared_ptr<Table> table, const RowData& row) = 0;
    virtual int UpdateData(std::shared_ptr<Table> table, const RowData& where_clause, const RowData& row) = 0;
    virtual std::vector<RowData> QueryData(std::shared_ptr<Table> table, const RowData& row) = 0;
    virtual std::vector<RowData> QueryData(std::shared_ptr<Table> table, const RowData& row, int page_size, int offset) = 0;

protected:
    std::string db_file_;
};

#endif // SQLPROCESSOR_H
