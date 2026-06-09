
#include <logger2.h>

#include "sqlite_database.h"
#include "db_utils.h"

SqliteDatabase::SqliteDatabase(std::string db_file)
    : db_file_(db_file)
    , table_name_("Users")
{
    sqlite3_open(db_file.c_str(), &sqlite_);

    CreateTableIfNotExists();
}

SqliteDatabase::~SqliteDatabase()
{
    sqlite3_close(sqlite_);
}

int SqliteDatabase::CreateTableIfNotExists() {
    std::string sql_cmd = "CREATE TABLE IF NOT EXISTS " + table_name_ +
                          "(id INTEGER AUTO_INCREMENT PRIMARY KEY," +
                          ToString(UserKey::Id) + " INTEGER UNIQUE," +
                          ToString(UserKey::Name) + " VARCHAR(100) NOT NULL," +
                          ToString(UserKey::Gender) + " CHAR(16) DEFAULT 'Female'," +
                          ToString(UserKey::Age) + " INTEGER DEFAULT 0," +
                          ToString(UserKey::ImagePath) + " TEXT," +
                          ToString(UserKey::Keypoint) + " BLOB)";

    int rc = sqlite3_exec(sqlite_, sql_cmd.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        LOGE("sqlite3_exec failed, errmsg = %s, cmd=<%s>\n", sqlite3_errmsg(sqlite_), sql_cmd.c_str());
    }
    return 0;
}

std::optional<int> SqliteDatabase::AddUser(const User& user) {
    std::string sql_cmd = "INSERT INTO " + table_name_ + " (" +
                          ToString(UserKey::Id) + ", " +
                          ToString(UserKey::Name) + ", " +
                          ToString(UserKey::Gender) + ", " +
                          ToString(UserKey::Age) + ", " +
                          ToString(UserKey::ImagePath) + ", " +
                          ToString(UserKey::Keypoint) + ") VALUES (?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(sqlite_, sql_cmd.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return -1;

    sqlite3_bind_int64(stmt, 1, user.id);
    sqlite3_bind_text(stmt, 2, user.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, ToString(user.gender).c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_int64(stmt, 4, user.age);
    sqlite3_bind_text(stmt, 5, user.image_path.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 6, user.keypoint.data(), static_cast<int>(user.keypoint.size() * sizeof(double)), SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOGE("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(sqlite_));
        return std::nullopt;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    return static_cast<int>(sqlite3_last_insert_rowid(sqlite_));;
}

size_t SqliteDatabase::DeleteUser(const UserCondition& condition) {    
    std::string sql_cmd = "DELETE FROM " + table_name_;
    if (!condition.empty()) {
        sql_cmd.append(" WHERE ");

        int i = 0;
        for (const auto& [key, value] : condition) {
            if (i > 0) {
                sql_cmd.append(" AND ");
            }
            sql_cmd.append(ToString(key)).append("=? ");
            i++;
        }
    }
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(sqlite_, sql_cmd.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return 0;

    int idx = 1;
    for (const auto& [key, value] : condition) {
        std::visit([&] (const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::vector<double>>) {
                sqlite3_bind_blob(stmt, idx, val.data(), static_cast<int>(val.size() * sizeof(double)), SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, Gender>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int64(stmt, idx, val);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else {
                // stmt.bind(val);
            }
        }, value);
        idx ++;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOGE("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(sqlite_));
        return -1;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return 0;
}

size_t SqliteDatabase::UpdateUser(const UserCondition& where, const UserCondition& values) {
    if (values.empty()) {
        return 0;
    }

    std::string sql_cmd = "UPDATE " + table_name_ + " SET ";

    int idx = 0;
    for (const auto& [key, value] : values) {
        if (idx > 0) {
            sql_cmd.append(", ");
        }
        sql_cmd.append(ToString(key)).append("=? ");
        idx ++;
    }

    idx = 0;
    if (!where.empty()) {
        sql_cmd.append(" WHERE ");

        for (const auto& [key, value] : where) {
            if (idx > 0) {
                sql_cmd.append(" AND ");
            }
            sql_cmd.append(ToString(key)).append("=? ");
            idx++;
        }
    }

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(sqlite_, sql_cmd.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return 0;
    }

    idx = 1;
    for (const auto& [key, value] : values) {
        std::visit([&] (const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::vector<double>>) {
                sqlite3_bind_blob(stmt, idx, val.data(), static_cast<int>(val.size() * sizeof(double)), SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, Gender>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int64(stmt, idx, val);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else {
                // stmt.bind(val);
            }
        }, value);
        idx ++;
    }

    for (const auto& [key, value] : where) {
        std::visit([&] (const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::vector<double>>) {
                sqlite3_bind_blob(stmt, idx, val.data(), static_cast<int>(val.size() * sizeof(double)), SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, Gender>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int64(stmt, idx, val);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else {
                // stmt.bind(val);
            }
        }, value);
        idx ++;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOGE("sqlite3_step failed, errmsg = %s\n", sqlite3_errmsg(sqlite_));
        return -1;
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return 0;
}

std::vector<User> SqliteDatabase::QueryUser(const UserCondition& condition, size_t page, size_t page_size) {
    std::vector<User> users;

    std::string sql_cmd = "SELECT * FROM " + table_name_;

    if (!condition.empty()) {
        sql_cmd.append(" WHERE ");

        int i = 0;
        for (const auto& [key, value] : condition) {
            if (i > 0) {
                sql_cmd.append(" AND ");
            }
            sql_cmd.append(ToString(key)).append("=? ");
            i++;
        }
    }

    sql_cmd.append(" ORDER BY id LIMIT ")
        .append(std::to_string(page_size))
        .append(" OFFSET ")
        .append(std::to_string(page * page_size));

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(sqlite_, sql_cmd.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
        return users;

    // 绑定
    int idx = 0;
    for (const auto& [key, value] : condition) {
        std::visit([&] (const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::vector<double>>) {
                sqlite3_bind_blob(stmt, idx, val.data(), static_cast<int>(val.size() * sizeof(double)), SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, Gender>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int64(stmt, idx, val);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, ToString(val).c_str(), -1, SQLITE_TRANSIENT);
            }
            else {
                // stmt.bind(val);
            }
        }, value);
        idx++;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        User user = CreateUser(stmt);
        users.push_back(user);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return users;
}

std::vector<User> SqliteDatabase::GetAllUsers(int limit) {
    std::vector<User> users;

    std::string sql_cmd = "SELECT * FROM " + table_name_ + " ORDER BY id";
    if (limit >= 0) {
        sql_cmd.append(" LIMIT ").append(std::to_string(limit));
    }
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(sqlite_, sql_cmd.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        LOGE("sqlite3_prepare_v2 failed, err= %s, sql=%s", sqlite3_errmsg(sqlite_), sql_cmd.c_str());
        return users;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        User user = CreateUser(stmt);
        users.push_back(user);
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    return users;
}

size_t SqliteDatabase::GetUserCount() {
    int count = 0;

    std::string sql_cmd = "SELECT COUNT(*) FROM " + table_name_;

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(sqlite_, sql_cmd.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return count;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0); // 第0列就是 COUNT(*)
    }
    sqlite3_finalize(stmt);
    return count;
}

User SqliteDatabase::CreateUser(sqlite3_stmt* stmt) {
    User user;
    int count = sqlite3_column_count(stmt);
    for (int col = 0; col < count; col++) {
        int type = sqlite3_column_type(stmt, col);
        if (type == SQLITE_NULL) {
            continue;
        }
        std::string col_name = sqlite3_column_name(stmt, col);
        if (col_name == ToString(UserKey::Name)) {
            const unsigned char* name = sqlite3_column_text(stmt, col);
            user.name = reinterpret_cast<const char*>(name);
        }
        else if (col_name == ToString(UserKey::Age)) {
            user.age = sqlite3_column_int64(stmt, col);
        }
        else if (col_name == ToString(UserKey::Gender)) {
            const unsigned char* gender = sqlite3_column_text(stmt, col);
            user.gender = ToGender(reinterpret_cast<const char*>(gender));
        }
        else if (col_name == ToString(UserKey::Id)) {
            user.id = sqlite3_column_int64(stmt, col);
        }
        else if (col_name == ToString(UserKey::ImagePath)) {
            const unsigned char* img_path = sqlite3_column_text(stmt, col);
            user.image_path = reinterpret_cast<const char*>(img_path);
        }
        else if (col_name == ToString(UserKey::Keypoint)) {
            const void* blob = sqlite3_column_blob(stmt, col);
            int size = sqlite3_column_bytes(stmt, col);

            if (blob && size > 0) {
                user.keypoint.assign(reinterpret_cast<const double*>(blob),
                                     reinterpret_cast<const double*>(blob) + size / sizeof(double));
            }
        }
    }
    return user;
}

int SqliteDatabase::Test() {
    return 0;
}
