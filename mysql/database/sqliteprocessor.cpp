#include <sstream>
#include "sqliteprocessor.h"
#include "logger.h"

SqliteProcessor::SqliteProcessor() {
}

SqliteProcessor::~SqliteProcessor() {

}
int SqliteProcessor::InitTable(const std::string& db_file, std::shared_ptr<Table> table) {
    int rc = sqlite3_open(db_file.c_str(), &handle_);
    if (rc != SQLITE_OK) {
        printf("sqlite3_open failed, errmsg = %s\n", sqlite3_errmsg(handle_));
        return -1;
    }

    std::string sql = create_createsql_cmd(table);
    // printf("%s:%d sql = %s\n", __func__, __LINE__, sql.c_str());

    rc = sqlite3_exec(handle_, sql.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        printf("sqlite3_exec failed, errmsg = %s, cmd=<%s>\n", sqlite3_errmsg(handle_), sql.c_str());
        return -1;
    }
    return 0;
}

int SqliteProcessor::GetRowCount(std::shared_ptr<Table> table) {
    std::string sql = "SELECT COUNT(*) FROM " + table->GetName();

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(handle_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return -1;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0); // 第0列就是 COUNT(*)
    }
    sqlite3_finalize(stmt);
    return count; //
}
int SqliteProcessor::InsertData(std::shared_ptr<Table> table, const RowData& row) {
    auto [fields, values] = get_field_value(table, row);

    std::string sql = create_insertsql_cmd(table, fields);
    // printf("%s:%d sql = %s\n", __func__, __LINE__, sql.c_str());

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(handle_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return -1;

    for (size_t i = 0; i < values.size(); ++i) {
        const auto& value = values[i];
        if (std::holds_alternative<std::nullptr_t>(value)) {
            sqlite3_bind_null(stmt, static_cast<int>(i + 1));
        } else if (std::holds_alternative<int64_t>(value)) {
            sqlite3_bind_int64(stmt, static_cast<int>(i + 1), std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            sqlite3_bind_double(stmt, static_cast<int>(i + 1), std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            const auto& str = std::get<std::string>(value);
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), str.c_str(), -1, SQLITE_TRANSIENT);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& blob = std::get<std::vector<double>>(value);
            sqlite3_bind_blob(stmt, static_cast<int>(i + 1), blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
        }
    }

    rc = sqlite3_step(stmt);

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        printf("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(handle_));
        return -1;
    }
    return 0;
}

int SqliteProcessor::DeleteData(std::shared_ptr<Table> table, const RowData& row) {
    auto [fields, values] = get_field_value(table, row);

    std::string sql = create_deletesql_cmd(table, fields);
    // printf("%s:%d sql = %s\n", __func__, __LINE__, sql.c_str());

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(handle_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return -1;

    for (size_t i = 0; i < values.size(); ++i) {
        const auto& value = values[i];
        if (std::holds_alternative<std::nullptr_t>(value)) {
            sqlite3_bind_null(stmt, static_cast<int>(i + 1));
        } else if (std::holds_alternative<int64_t>(value)) {
            sqlite3_bind_int64(stmt, static_cast<int>(i + 1), std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            sqlite3_bind_double(stmt, static_cast<int>(i + 1), std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            const auto& str = std::get<std::string>(value);
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), str.c_str(), -1, SQLITE_TRANSIENT);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& blob = std::get<std::vector<double>>(value);
            sqlite3_bind_blob(stmt, static_cast<int>(i + 1), blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
        }
    }

    rc = sqlite3_step(stmt);

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        printf("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(handle_));
        return -1;
    }
    return 0;
}

std::vector<SqlProcessor::RowData> SqliteProcessor::QueryData(std::shared_ptr<Table> table, const RowData& row) {
    std::vector<RowData> query_data;
    auto [fields, values] = get_field_value(table, row);

    std::string sql = create_querysql_cmd(table, fields);
    // printf("%s:%d sql = %s\n", __func__, __LINE__, sql.c_str());

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(handle_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return query_data;

    for (size_t i = 0; i < values.size(); ++i) {
        const auto& value = values[i];
        if (std::holds_alternative<std::nullptr_t>(value)) {
            sqlite3_bind_null(stmt, static_cast<int>(i + 1));
        }
        else if (std::holds_alternative<int64_t>(value)) {
            sqlite3_bind_int64(stmt, static_cast<int>(i + 1), std::get<int64_t>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            sqlite3_bind_double(stmt, static_cast<int>(i + 1), std::get<double>(value));
        }
        else if (std::holds_alternative<std::string>(value)) {
            const auto& str = std::get<std::string>(value);
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), str.c_str(), -1, SQLITE_TRANSIENT);
        }
        else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& blob = std::get<std::vector<double>>(value);
            sqlite3_bind_blob(stmt, static_cast<int>(i + 1), blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
        }
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        RowData query_row;
        int column_count = sqlite3_column_count(stmt);
        for (int col_idx = 0; col_idx < column_count; col_idx++) {
            int type = sqlite3_column_type(stmt, col_idx);
            if (type == SQLITE_NULL) {
                continue;
            }
            const char* column_name = sqlite3_column_name(stmt, col_idx);

            switch (type) {
            case SQLITE_INTEGER:
                query_row[column_name] =  sqlite3_column_int64(stmt, col_idx);
                break;
            case SQLITE_FLOAT:
                query_row[column_name] = sqlite3_column_double(stmt, col_idx);
                break;
            case SQLITE_TEXT: {
                const unsigned char* text = sqlite3_column_text(stmt, col_idx);
                query_row[column_name] =  text ? std::string(reinterpret_cast<const char*>(text)) : std::string();
                break;
            }
            case SQLITE_BLOB: {
                const void* blob = sqlite3_column_blob(stmt, col_idx);
                int size = sqlite3_column_bytes(stmt, col_idx);
                if (blob && size > 0) {
                    query_row[column_name] = std::vector<double>(
                        static_cast<const double*>(blob),
                        static_cast<const double*>(blob) + size
                        );
                }
                else {
                    query_row[column_name] = std::vector<double>{};
                }
                break;
            }
            default:
                break;
            }
        }
        query_data.push_back(query_row);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        LOGE("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(handle_));
        return query_data;
    }

    return query_data;
}

std::vector<SqlProcessor::RowData> SqliteProcessor::QueryData(std::shared_ptr<Table> table, const RowData& row, int page_size, int offset) {
    std::vector<RowData> query_data;
    auto [fields, values] = get_field_value(table, row);

    std::string sql = create_querysql_cmd(table, fields, page_size, offset);

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(handle_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return query_data;

    for (size_t i = 0; i < values.size(); ++i) {
        const auto& value = values[i];
        if (std::holds_alternative<std::nullptr_t>(value)) {
            sqlite3_bind_null(stmt, static_cast<int>(i + 1));
        }
        else if (std::holds_alternative<int64_t>(value)) {
            sqlite3_bind_int64(stmt, static_cast<int>(i + 1), std::get<int64_t>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            sqlite3_bind_double(stmt, static_cast<int>(i + 1), std::get<double>(value));
        }
        else if (std::holds_alternative<std::string>(value)) {
            const auto& str = std::get<std::string>(value);
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), str.c_str(), -1, SQLITE_TRANSIENT);
        }
        else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& blob = std::get<std::vector<double>>(value);
            sqlite3_bind_blob(stmt, static_cast<int>(i + 1), blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
        }
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        RowData query_row;
        int column_count = sqlite3_column_count(stmt);
        for (int col_idx = 0; col_idx < column_count; col_idx++) {
            int type = sqlite3_column_type(stmt, col_idx);
            if (type == SQLITE_NULL) {
                continue;
            }
            const char* column_name = sqlite3_column_name(stmt, col_idx);

            switch (type) {
            case SQLITE_INTEGER:
                query_row[column_name] =  sqlite3_column_int64(stmt, col_idx);
                break;
            case SQLITE_FLOAT:
                query_row[column_name] = sqlite3_column_double(stmt, col_idx);
                break;
            case SQLITE_TEXT: {
                const unsigned char* text = sqlite3_column_text(stmt, col_idx);
                query_row[column_name] =  text ? std::string(reinterpret_cast<const char*>(text)) : std::string();
                break;
            }
            case SQLITE_BLOB: {
                const void* blob = sqlite3_column_blob(stmt, col_idx);
                int size = sqlite3_column_bytes(stmt, col_idx);
                if (blob && size > 0) {
                    query_row[column_name] = std::vector<double>(
                        static_cast<const double*>(blob),
                        static_cast<const double*>(blob) + size
                        );
                }
                else {
                    query_row[column_name] = std::vector<double>{};
                }
                break;
            }
            default:
                break;
            }
        }
        query_data.push_back(query_row);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        printf("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(handle_));
        return query_data;
    }
    return query_data;
}

int SqliteProcessor::UpdateData(std::shared_ptr<Table> table, const RowData& where_clause, const RowData& row) {
    auto [fields, values] = get_field_value(table, row);
    auto [where_fields, where_values] = get_field_value(table, where_clause);

    std::string sql = create_updatesql_cmd(table, fields, where_fields);
    printf("%s:%d sql = %s\n", __func__, __LINE__, sql.c_str());

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(handle_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return -1;

    for (size_t i = 0; i < values.size(); ++i) {
        const auto& value = values[i];
        if (std::holds_alternative<std::nullptr_t>(value)) {
            sqlite3_bind_null(stmt, static_cast<int>(i + 1));
        } else if (std::holds_alternative<int64_t>(value)) {
            sqlite3_bind_int64(stmt, static_cast<int>(i + 1), std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            sqlite3_bind_double(stmt, static_cast<int>(i + 1), std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            const auto& str = std::get<std::string>(value);
            sqlite3_bind_text(stmt, static_cast<int>(i + 1), str.c_str(), -1, SQLITE_TRANSIENT);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& blob = std::get<std::vector<double>>(value);
            sqlite3_bind_blob(stmt, static_cast<int>(i + 1), blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
        }
    }
    for (size_t i = 0; i < where_values.size(); ++i) {
        const auto& value = where_values[i];
        int bind_idx = static_cast<int>(values.size() + i + 1);

        if (std::holds_alternative<std::nullptr_t>(value)) {
            sqlite3_bind_null(stmt, bind_idx);
        }
        else if (std::holds_alternative<int64_t>(value)) {
            sqlite3_bind_int64(stmt, bind_idx, std::get<int64_t>(value));
        }
        else if (std::holds_alternative<double>(value)) {
            sqlite3_bind_double(stmt, bind_idx, std::get<double>(value));
        }
        else if (std::holds_alternative<std::string>(value)) {
            const std::string& str = std::get<std::string>(value);
            sqlite3_bind_text(stmt, bind_idx, str.c_str(), -1, SQLITE_TRANSIENT);
        }
        else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& blob = std::get<std::vector<double>>(value);
            sqlite3_bind_blob(stmt, bind_idx, blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
        }
    }

    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        printf("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(handle_));
        return -1;
    }
    return 0;
}

std::string SqliteProcessor::create_createsql_cmd(std::shared_ptr<Table> table) {
    std::stringstream stream;
    const std::vector<std::string> field = table->GetField();
    const std::vector<FieldType> type = table->GetType();

    if (field.empty() || field.size() != type.size()) {
        printf("field & type error..\n");
        return stream.str();
    }

    stream << "CREATE TABLE IF NOT EXISTS " << table->GetName() << " "
           << "(id INTEGER PRIMARY KEY AUTOINCREMENT, ";
    for (int idx = 0; idx < field.size(); idx++) {
        if (idx == (field.size() - 1)) {
            stream << field[idx] << " " << Table::GetString(type[idx]) << ");";
        }
        else {
            stream << field[idx] << " " << Table::GetString(type[idx]) << ", ";
        }
    }
    return stream.str();
}

std::string SqliteProcessor::create_insertsql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields) {
    if (fields.empty()) {
        return std::string();
    }

    // INSERT INTO %s (name, age) VALUES (?, ?);
    std::stringstream stream;
    stream << "INSERT INTO " << table->GetName() << " ";
    stream << "(";
    for (int idx = 0; idx < fields.size(); idx++) {
        stream << fields[idx];
        if (idx + 1 < fields.size()) {
            stream << ", ";
        }
    }
    stream << ")";
    stream << " VALUES ";
    stream << "(";
    for (int idx = 0; idx < fields.size(); idx++) {
        stream << "?";
        if (idx + 1 < fields.size()) {
            stream << ", ";
        }
    }
    stream << ");";
    return stream.str();
}

std::string SqliteProcessor::create_deletesql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields) {
    // DELETE FROM student_test_new WHERE class='grade_0' AND age=10;
    std::stringstream stream;
    stream << "DELETE FROM " << table->GetName();
    if (fields.size() > 0) {
        stream << " WHERE ";
    }
    for (int idx = 0; idx < fields.size(); idx++) {
        stream << fields[idx] << " = ? ";
        if (idx + 1 < fields.size()) {
            stream << " AND ";
        }
    }
    stream << ";";
    return stream.str();
}

//
std::string SqliteProcessor::create_querysql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields) {
    // SELECT id, name, class, age FROM student
    //      WHRER class='grade_0' AND age= 10
    std::stringstream stream;
    stream << "SELECT * FROM " << table->GetName();

    if (fields.size() > 0) {
        stream << " WHERE ";
    }
    for (int idx = 0; idx < fields.size(); idx++) {
        stream << fields[idx] << " = ? ";
        if (idx + 1 < fields.size()) {
            stream << " AND ";
        }
    }
    stream << ";";
    return stream.str();
}

std::string SqliteProcessor::create_querysql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields, int page_size, int offset) {
    // SELECT id, name, class, age FROM student
    //      WHRER class='grade_0' AND age= 10 ORDER BY id LIMIT 5 OFFSET 10
    std::stringstream stream;
    stream << "SELECT * FROM " << table->GetName();

    if (fields.size() > 0) {
        stream << " WHERE ";
    }
    for (int idx = 0; idx < fields.size(); idx++) {
        stream << fields[idx] << " = ? ";
        if (idx + 1 < fields.size()) {
            stream << " AND ";
        }
    }
    stream << " ORDER BY id LIMIT " << page_size << " OFFSET " << offset << " ";
    stream << ";";
    return stream.str();
}

std::string SqliteProcessor::create_updatesql_cmd(std::shared_ptr<Table> table, const std::vector<std::string>& fields, const std::vector<std::string>& where_fields) {
    if (fields.empty()) {
        return std::string();
    }
    // UPDATE student_test_new
    //     SET name = 'liushiyao', age = 10     // SET 用 ， 分割多个赋值
    //     WHERE name = 'lsy' AND age = 12;     // 用逻辑语句 AND / OR 连接多个条件

    std::stringstream stream;
    stream << "UPDATE " << table->GetName() << " SET ";
    for (int idx = 0; idx < fields.size(); idx++) {
        stream << fields[idx] << " = ?";
        if (idx + 1 < fields.size()) {
            stream << ", ";
        }
    }

    if (where_fields.size() > 0) {
        stream << " WHERE ";
        for (int idx  = 0; idx < where_fields.size(); idx++) {
            stream << where_fields[idx] << " = ?";
            if (idx + 1 < where_fields.size()) {
                stream << " AND ";
            }
        }
    }
    stream <<";";
    return stream.str();
}

std::pair<std::vector<std::string>,std::vector<FieldValue>>
SqliteProcessor::get_field_value(std::shared_ptr<Table> table, const RowData& row) {
    std::vector<std::string> field;
    std::vector<FieldValue> value;

    for (const auto& it : table->GetField()) {
        auto find = row.find(it);
        if (find != row.end()) {
            field.push_back(find->first);
            value.push_back(find->second);
        }
    }
    return std::make_pair(field, value);
}
