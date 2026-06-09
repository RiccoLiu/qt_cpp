

# Connector c++ 安装

```
vcpkg install mysql-connector-cpp:x64-windows 
```

# 编码

## SQL 模式

```

// 1. 运行SQL指令时建议使用try、catch 获取异常的方式

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

// 2. 对于查询语句建议使用绑定参数的方式，防止SQL注入风险
std::optional<int> MysqlDatabase::AddUser(const User& user) {
    try {
        std::string sql_cmd = "INSERT INTO " + table_name_ + " (" +
                              ToString(UserKey::Id) + ", " +
                              ToString(UserKey::Name) + ", " +
                              ToString(UserKey::Gender) + ", " +
                              ToString(UserKey::Age) + ", " +
                              ToString(UserKey::ImagePath) + ", " +
                              ToString(UserKey::Keypoint) + ") VALUES (?, ?, ?, ?, ?, ?)";

        // 参数绑定
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

// 3. 对于查询语句需要使用 fetchOne 或者fetchAll 后才能使用， 从查询结果获取 字段
......
        mysqlx::SqlResult res = sess_->sql(sql_cmd).execute();
        auto rows = res.fetchAll();

        // 获取字段映射到列数的映射
        std::map<std::string, size_t> col_map;
        for (size_t i = 0; i < res.getColumnCount(); ++i) {
            std::string label = res.getColumn(i).getColumnLabel();
            col_map[label] = i;
        }
......

// 4. 小工具

// 4.1. BLOB数据序列化
template<typename T>
std::string SerializeVectorAsBlob(const std::vector<T>& vec) {
    // 确保 T 是平凡可复制的（POD-like），避免对 string、vector 等复杂类型误用
    static_assert(
        std::is_trivially_copyable_v<T>,
        "to_blob only supports trivially copyable types (e.g., int, float, double)"
        );

    if (vec.empty()) {
        return {};
    }

    return std::string(
        reinterpret_cast<const char*>(vec.data()),
        vec.size() * sizeof(T)
        );
}

// 4.2. mysqlx::Row解序列化
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

```

mysqlx::SqlResult 相关函数

- getAutoIncrementValue: 获取 自增主键 的值
- getAffectedItemsCount: 获取 受影响行数
