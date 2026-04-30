#include <iostream>
#include <vector>

#include <sqlite3.h>

struct Student {
    std::string name;
    int age;
    std::string photo;
};


int AddColumn() {

    return 0;
}

int main() {
    std::cout << "test::main::Hello World" << std::endl;
#if 1
    std::string db_file_name = "school.db";
    std::string table_name = "student";

    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;

    printf("--- sqlite test start -----\n");

    // 1. 打开数据库
    int rc = sqlite3_open(db_file_name.c_str(), &db);
    if (rc != SQLITE_OK) {
        printf("sqlite3_open failed, sqlite3_errmsg: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("sqlite3_open rc: %d\n", rc);

    // 2. 建表
    char create_sql[256];
    snprintf(create_sql, sizeof(create_sql),
             "CREATE TABLE IF NOT EXISTS %s("
             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
             "name TEXT NOT NULL,"
             "age INTEGER,"
             "keypoint DOUBLE,"
             "image BLOB);",
             table_name.c_str());

    char *errmsg = nullptr;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        printf("sqlite3_exec failed, sqlite3_errmsg: %s, errmsg: %s\n", sqlite3_errmsg(db), errmsg);
        return -1;
    }
    printf("sqlite3_exec rc: %d\n", rc);

    std::vector<Student> students = {
        {"lc", 33, "/home/lc.png"}, // {} 聚合初始化
        {"bry", 32, "/home/bry.png"},
        {"lsy", 3, "/home/lsy.png"}
    };

    // 3. 插入数据 - 使用预编译指令 + 参数绑定 (防止SQL注入)
    for (const auto& student : students) {
        char insert_sql[256];
        snprintf(insert_sql, sizeof(insert_sql),
                 "INSERT INTO %s(name, age) VALUES (?, ?);", table_name.c_str());

        // 预处理指令
        rc = sqlite3_prepare_v2(db, insert_sql, sizeof(insert_sql), &stmt, nullptr);
        if (rc != SQLITE_OK) {
            printf("sqlite3_prepare_v2 failed, sqlite3_errmsg: %s\n", sqlite3_errmsg(db));
            return -1;
        }
        printf("sqlite3_prepare_v2 rc: %d\n", rc);

        // 绑定参数
        sqlite3_bind_text(stmt, 1, student.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, student.age);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            printf("sqlite3_step failed, rc: %d, sqlite3_errmsg: %s\n", rc, sqlite3_errmsg(db));
            return -1;
        }
        printf("sqlite3_step rc: %d\n", rc);

        // 清除绑定信息
        sqlite3_reset(stmt);
    }

    // 释放插入语句资源
    sqlite3_finalize(stmt);
    stmt = NULL;

    // 4. 查询信息
    char select_sql[256];
    snprintf(select_sql, sizeof(select_sql),
             "SELECT * FROM %s WHERE age > ?;", table_name.c_str());

    rc = sqlite3_prepare_v2(db, select_sql, sizeof(select_sql), &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("sqlite3_prepare_v2 failed, sqlite3_errmsg: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("sqlite3_prepare_v2 rc: %d\n", rc);

    sqlite3_bind_int(stmt, 1, 2);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        double keypoint = sqlite3_column_double(stmt, 3);

        // sqlite3_column_blob(stmt, 4);

        printf("id: %d, name: %s, age: %d, keypoint: %f\n",
               id, name, age, keypoint);
    }



    // 释放查询语句的资源
    sqlite3_finalize(stmt);
    stmt = NULL;


    sqlite3_close(db);
    db = NULL;
#endif
    return 0;
}

#if 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// 辅助函数：检查 SQLite 返回码并打印错误
static void check_rc(sqlite3* db, int rc, const char* msg) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        fprintf(stderr, "Error (%s): %s\n", msg, sqlite3_errmsg(db));
        exit(1);
    }
}

int main() {
    sqlite3* db = NULL;
    sqlite3_stmt* stmt = NULL;
    int rc;

    printf("=== SQLite C API 生命周期示例 ===\n");

    // ────────────────────────────────
    // 1. 打开数据库连接（创建或打开文件）
    // ────────────────────────────────
    rc = sqlite3_open("school.db", &db);
    check_rc(db, rc, "sqlite3_open");
    printf("✅ 数据库已打开/创建: school.db\n");

    // ────────────────────────────────
    // 2. 执行 DDL（建表）—— 使用 sqlite3_exec（简单语句）
    // ────────────────────────────────
    const char* create_sql =
        "CREATE TABLE IF NOT EXISTS students ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "age INTEGER);";

    rc = sqlite3_exec(db, create_sql, NULL, NULL, NULL);
    check_rc(db, rc, "CREATE TABLE");
    printf("✅ 表 students 已创建\n");

    // ────────────────────────────────
    // 3. 插入数据 —— 使用预编译语句 + 参数绑定（防止 SQL 注入）
    // ────────────────────────────────
    const char* insert_sql = "INSERT INTO students (name, age) VALUES (?, ?);";

    // 3.1 准备语句（生命周期开始）
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);
    check_rc(db, rc, "sqlite3_prepare_v2 INSERT");
    printf("✅ 插入语句已准备\n");

    // 3.2 绑定参数并执行第一条记录
    sqlite3_bind_text(stmt, 1, "Alice", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, 22);
    rc = sqlite3_step(stmt);
    check_rc(db, rc, "sqlite3_step INSERT Alice");
    printf("✅ 插入 Alice\n");

    // 3.3 重置语句（清除绑定，准备下一次执行）
    sqlite3_reset(stmt);

    // 3.4 绑定第二条记录
    sqlite3_bind_text(stmt, 1, "Bob", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, 19);
    rc = sqlite3_step(stmt);
    check_rc(db, rc, "sqlite3_step INSERT Bob");
    printf("✅ 插入 Bob\n");

    // 3.5 重置（可选，但好习惯）
    sqlite3_reset(stmt);

    // 3.6 【关键】释放语句资源（生命周期结束）
    sqlite3_finalize(stmt);
    stmt = NULL; // 避免悬空指针
    printf("✅ 插入语句已释放\n");

    // ────────────────────────────────
    // 4. 查询数据 —— 使用预编译语句 + 获取列值
    // ────────────────────────────────
    const char* select_sql = "SELECT name, age FROM students WHERE age > ?;";

    // 4.1 准备查询语句
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL);
    check_rc(db, rc, "sqlite3_prepare_v2 SELECT");
    printf("\n✅ 查询语句已准备\n");

    // 4.2 绑定查询条件
    sqlite3_bind_int(stmt, 1, 20);

    // 4.3 循环获取结果行
    printf("🔍 查询 age > 20 的学生:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(stmt, 0);
        int age = sqlite3_column_int(stmt, 1);
        printf("  - %s, %d 岁\n", name ? name : "(null)", age);
    }
    check_rc(db, rc, "sqlite3_step SELECT loop");

    // 4.4 【关键】释放查询语句
    sqlite3_finalize(stmt);
    stmt = NULL;
    printf("✅ 查询语句已释放\n");

    // ────────────────────────────────
    // 5. 关闭数据库连接（最后一步！）
    // ────────────────────────────────
    sqlite3_close(db);
    db = NULL;
    printf("\n✅ 数据库连接已关闭\n");

    printf("🎉 示例运行完成！查看 school.db 文件。\n");
    return 0;
}

#endif
