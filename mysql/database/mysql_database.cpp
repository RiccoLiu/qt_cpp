#include <iostream>
#include <logger2.h>

#include "mysql_database.h"
#include "db_utils.h"

MysqlDatabase::MysqlDatabase(std::unique_ptr<mysqlx::Session> sess, mysqlx::Schema schema)
    : sess_(std::move(sess))
    , schema_(schema)
    , table_name_("Users")
{
    CreateTableIfNotExists();
}

MysqlDatabase::~MysqlDatabase()
{
}

int MysqlDatabase::CreateTableIfNotExists() {
    try {
        std::string sql_cmd = "CREATE TABLE IF NOT EXISTS " + table_name_ +
                              "(id INTEGER AUTO_INCREMENT PRIMARY KEY," +
                              ToString(UserKey::Id) + " INTEGER UNIQUE," +
                              ToString(UserKey::Name) + " VARCHAR(100) NOT NULL," +
                              ToString(UserKey::Gender) + " CHAR(16) DEFAULT 'Female'," +
                              ToString(UserKey::Age) + " INTEGER DEFAULT 0," +
                              ToString(UserKey::ImagePath) + " TEXT," +
                              ToString(UserKey::Keypoint) + " BLOB)";
        LOGI("sql_cmd: %s", sql_cmd.c_str());

        sess_->sql(sql_cmd).execute();
    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return 0;
}

std::optional<int> MysqlDatabase::AddUser(const User& user) {
    try {
        std::string sql_cmd = "INSERT INTO " + table_name_ + " (" +
                              ToString(UserKey::Id) + ", " +
                              ToString(UserKey::Name) + ", " +
                              ToString(UserKey::Gender) + ", " +
                              ToString(UserKey::Age) + ", " +
                              ToString(UserKey::ImagePath) + ", " +
                              ToString(UserKey::Keypoint) + ") VALUES (?, ?, ?, ?, ?, ?)";
        LOGI("sql_cmd: %s", sql_cmd.c_str());

        mysqlx::SqlResult res = sess_->sql(sql_cmd)
            .bind(user.id, user.name, ToString(user.gender), user.age, user.image_path, SerializeVectorAsBlob(user.keypoint))
            .execute();

        int64_t inserted_id = res.getAutoIncrementValue();
        return static_cast<int>(inserted_id);

    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return std::nullopt;
}

size_t MysqlDatabase::DeleteUser(const UserCondition& condition) {
    try {
        if (condition.empty()) {
            // DELETE FROM sudent
            auto result = sess_->sql("DELETE FROM " + table_name_).execute();
            return result.getAffectedItemsCount();
        }
        else {
            // DELETE FROM student WHERE class='grade_0' AND name='lc'
            std::string sql_cmd = "DELETE FROM " + table_name_ + " WHERE ";

            int idx = 0;
            for (const auto& [key, value] : condition) {
                if (idx > 0) {
                    sql_cmd.append(" AND ");
                }

                sql_cmd.append(ToString(key)).append("=? ");
                idx++;
            }
            LOGI("sql_cmd: %s", sql_cmd.c_str());

            mysqlx::SqlStatement stmt = sess_->sql(sql_cmd);

            for (const auto& [key, value] : condition) {
                std::visit([&stmt](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::vector<double>>) {
                        auto blob = SerializeVectorAsBlob(val);
                        stmt.bind(mysqlx::bytes(reinterpret_cast<const mysqlx::byte*>(blob.data()), blob.size()));
                    }
                    else if constexpr (std::is_same_v<T, Gender>) {
                        stmt.bind(ToString(val));
                    }
                    else {
                        stmt.bind(val);
                    }
                }, value);
            }

            auto result = stmt.execute();
            return result.getAffectedItemsCount();
        }
    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return 0;
}

size_t MysqlDatabase::UpdateUser(const UserCondition& where, const UserCondition& values) {
    try {
        if (values.empty()) {
            return 0;
        }

        // UPDATE student_test_new
        //     SET name = 'liushiyao', age = 10     // SET 用 , 分割多个赋值
        //     WHERE name = 'lsy' AND age = 12;     // 用逻辑语句 AND / OR 连接多个条件

        std::string sql_cmd = "UPDATE " + table_name_ + " SET ";

        int idx = 0;
        for (const auto& [key, value] : values) {
            if (idx > 0) {
                sql_cmd.append(",");
            }
            sql_cmd.append(ToString(key)).append("= ?");
            idx++;
        }

        if (!where.empty()) {
            sql_cmd.append(" WHERE ");

            idx = 0;
            for (const auto& [key, value] : where) {
                if (idx > 0) {
                    sql_cmd.append(" AND ");
                }
                sql_cmd.append(ToString(key)).append("=?");
                idx++;
            }
        }
        mysqlx::SqlStatement stmt = sess_->sql(sql_cmd);

        for (const auto& [key, value] : values) {
            std::visit([&stmt] (const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::vector<double>>) {
                    auto blob = SerializeVectorAsBlob(val);
                    stmt.bind(mysqlx::bytes(reinterpret_cast<const mysqlx::byte*>(blob.data()), blob.size()));
                }
                else if constexpr (std::is_same_v<T, Gender>) {
                    stmt.bind(ToString(val));
                }
                else {
                    stmt.bind(val);
                }
            }, value);
        }

        if (!where.empty()) {
            for (const auto& [key, value] : where) {
                std::visit([&stmt] (const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::vector<double>>) {
                        auto blob = SerializeVectorAsBlob(val);
                        stmt.bind(mysqlx::bytes(reinterpret_cast<const mysqlx::byte*>(blob.data()), blob.size()));
                    }
                    else if constexpr (std::is_same_v<T, Gender>) {
                        stmt.bind(ToString(val));
                    }
                    else {
                        stmt.bind(val);
                    }
                }, value);
            }
        }

        mysqlx::SqlResult res = stmt.execute();
        return res.getAffectedItemsCount();
    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return 0;
}

std::vector<User> MysqlDatabase::QueryUser(const UserCondition& condition, size_t page, size_t page_size) {
    std::vector<User> users;
    try {
        // SELECT * FROM student
        //      WHERE class='grade_0' AND age= 10 ORDER BY id LIMIT 5 OFFSET 10

        std::string sql_cmd = "SELECT * FROM " + table_name_;

        if (!condition.empty()) {
            sql_cmd.append(" WHERE ");

            int idx = 0;
            for (const auto& [key, value] : condition) {
                if (idx > 0) {
                    sql_cmd.append(" AND ");
                }

                sql_cmd.append(ToString(key)).append("=? ");
                idx++;
            }
        }

        sql_cmd.append(" ORDER BY id LIMIT ")
               .append(std::to_string(page_size))
               .append(" OFFSET ")
               .append(std::to_string(page * page_size));

        LOGI("sql_cmd: %s", sql_cmd.c_str());

        mysqlx::SqlStatement stmt = sess_->sql(sql_cmd);

        for (const auto& [key, value] : condition) {
            std::visit([&stmt](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::vector<double>>) {
                    auto blob = SerializeVectorAsBlob(val);
                    stmt.bind(mysqlx::bytes(reinterpret_cast<const mysqlx::byte*>(blob.data()), blob.size()));
                }
                else if constexpr (std::is_same_v<T, Gender>) {
                    stmt.bind(ToString(val));
                }
                else {
                    stmt.bind(val);
                }
            }, value);
        }

        mysqlx::SqlResult res = stmt.execute();
        auto rows = res.fetchAll();

        // 获取字段映射到列数的映射
        std::map<std::string, size_t> col_map;
        for (size_t i = 0; i < res.getColumnCount(); ++i) {
            std::string label = res.getColumn(i).getColumnLabel();
            col_map[label] = i;
        }

        for (const mysqlx::Row& row : rows) {
            User user = CreateUser(row, col_map);
            users.push_back(user);
        }
    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return users;
}

std::vector<User> MysqlDatabase::GetAllUsers(int limit) {
    std::vector<User> users;
    try {
        std::string sql_cmd = "SELECT * FROM " + table_name_ + " ORDER BY id";
        if (limit >= 0) {
            sql_cmd.append(" LIMIT ").append(std::to_string(limit));
        }
        LOGI("sql_cmd: %s", sql_cmd.c_str());

        mysqlx::SqlResult res = sess_->sql(sql_cmd).execute();
        auto rows = res.fetchAll();

        // 获取字段映射到列数的映射
        std::map<std::string, size_t> col_map;
        for (size_t i = 0; i < res.getColumnCount(); ++i) {
            std::string label = res.getColumn(i).getColumnLabel();
            col_map[label] = i;
        }

        for (const mysqlx::Row& row : rows) {
            User user = CreateUser(row, col_map);
            users.push_back(user);
        }
    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return users;
}

size_t MysqlDatabase::GetUserCount() {
    try {
        // SELECT COUNT(*) FROM table_name
        std::string sql_cmd = "SELECT COUNT(*) FROM " + table_name_;
        LOGI("sql_cmd:%s", sql_cmd.c_str());

        mysqlx::SqlResult res = sess_->sql(sql_cmd).execute();
        auto row = res.fetchOne(); // ✅ 安全获取唯一行

        if (!row.isNull()) {
            return static_cast<size_t>(row[0].get<int>());
        }
    } catch (const mysqlx::Error& e) {
        LOGE("MySQL Error: %s", e.what());
    } catch (const std::exception& e) {
        LOGE("MySQL Exception: %s", e.what());
    }
    return 0;
}

User MysqlDatabase::CreateUser(const mysqlx::Row& row, const std::map<std::string, size_t>& col_map) {
    User user;

    if (auto it = col_map.find(ToString(UserKey::Id)); it != col_map.end())
        user.id = row[it->second].get<int>();
    if (auto it = col_map.find(ToString(UserKey::Name)); it != col_map.end())
        user.name = row[it->second].get<std::string>();
    if (auto it = col_map.find(ToString(UserKey::Gender)); it != col_map.end())
        user.gender = ToGender(row[it->second].get<std::string>());
    if (auto it = col_map.find(ToString(UserKey::Age)); it != col_map.end())
        user.age = row[it->second].get<int>();
    if (auto it = col_map.find(ToString(UserKey::ImagePath)); it != col_map.end())
        user.image_path = row[it->second].get<std::string>();

    if (auto it = col_map.find(ToString(UserKey::Keypoint)); it != col_map.end()) {
        auto val = row[it->second];
        if (!val.isNull()) {
            auto [ptr, size] = val.get<mysqlx::bytes>();
            if (size % sizeof(double) == 0 && size > 0) {
                user.keypoint.assign(
                    reinterpret_cast<const double*>(ptr),
                    reinterpret_cast<const double*>(ptr) + size / sizeof(double)
                    );
            }
        }
    }
    return user;
}

int MysqlDatabase::Test() {
    LOGI("==== MysqlDatabase::Test ====");

    LOGI("GetUserCount() = %u", GetUserCount());
#if 1
    {
        User lc;
        lc.id = 13567;
        lc.name = "lc";
        lc.gender = Gender::Male;
        lc.age = 32;
        lc.image_path = "/home/lc.png";

        std::optional<int> ret = AddUser(lc);
        if (ret) {
            LOGI("AddUser lc success, ret = %d", ret.value());
        } else {
            LOGI("AddUser lc failed..");
        }

        std::vector<User> users = GetAllUsers();
        LOGI("GetAllUsers size = %d", users.size());
        int i = 0;
        for (const auto& it: users) {
            LOGI("[%s:%d] -------- 1111 ------- i: %d, user_id: %d, name: %s, gender: %s, age: %d, image_path: %s", __func__, __LINE__, i,
                 it.id, it.name.c_str(), ToString(it.gender).c_str(), it.age, it.image_path.c_str());
            i++;
        }

        std::vector<User> users2 = QueryUser(UserCondition());
        LOGI("QueryUser size = %d", users.size());
        i = 0;
        for (const auto& it: users2) {
            LOGI("[%s:%d]-------- 1111 -------i: %d, user_id: %d, name: %s, gender: %s, age: %d, image_path: %s", __func__, __LINE__, i,
                 it.id, it.name.c_str(), ToString(it.gender).c_str(), it.age, it.image_path.c_str());
            i++;
        }
    }
    {
        User bry;
        bry.id = 13569;
        bry.name = "bry";
        bry.gender = Gender::Female;
        bry.age = 32;
        bry.image_path = "/home/bry.png";

        std::optional<int> ret = AddUser(bry);
        if (ret) {
            LOGI("AddUser bry success, ret = %d", ret.value());
        } else {
            LOGI("AddUser bry failed..");
        }

        std::vector<User> users3 = GetAllUsers();
        LOGI("GetAllUsers size = %d", users3.size());
        int i = 0;
        for (const auto& it: users3) {
            LOGI("[%s:%d]-------- 2222 -------i: %d, user_id: %d, name: %s, gender: %s, age: %d, image_path: %s", __func__, __LINE__, i,
                 it.id, it.name.c_str(), ToString(it.gender).c_str(), it.image_path.c_str());
            i++;
        }

        std::vector<User> users4 = QueryUser(UserCondition());
        LOGI("QueryUser size = %d", users4.size());
        i = 0;
        for (const auto& it: users4) {
            LOGI("[%s:%d]-------- 2222 -------i: %d, user_id: %d, name: %s, gender: %s, age: %d, image_path: %s", __func__, __LINE__, i,
                 it.id, it.name.c_str(), ToString(it.gender).c_str(), it.age, it.image_path.c_str());
            i++;
        }
    }

    // DeleteUser(UserCondition());
#else
    std::vector<User> users = GetAllUsers();
    LOGI("GetAllUsers size = %d", users.size());
    int i = 0;
    for (const auto& user: users) {
        LOGI("i: %d, user_id: %d, name: %s, gender: %s, age: %d, image_path: %s", i,
             user.id, user.name.c_str(), ToString(user.gender).c_str(), user.age, user.image_path.c_str());
        i++;
    }
#endif
    return 0;
}

