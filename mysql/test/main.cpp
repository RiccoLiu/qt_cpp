#include <iostream>
#include <vector>

#include <logger2.h>
#include <sqlite3.h>            // sqlite3 头文件
#include <Eigen/Core>           // Eigen
#include <yaml-cpp/yaml.h>
#include <mysqlx/xdevapi.h>     // mysql connector 头文件
#include <opencv2/opencv.hpp>   // opencv 头文件

int sqlite3_test() {
    struct Student {
        std::string name;
        int age;
        std::string photo;
    };

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

    return 0;
}

int mysql_connector_test() {
    struct Student {
        std::string name;
        int age;
        std::string photo;
    };

    std::vector<Student> students = {
        {"lc", 33, "/home/lc.png"},
        {"bry", 32, "/home/bry.png"},
        {"lsy", 3, "/home/lsy.png"}
    };


    try {
        // 1. 连接服务器
        struct SessionInfo {
            std::string host;
            int port;

            std::string user;
            std::string passwd;
            std::string database;

            std::string get_uri() const {
                return std::string("mysqlx://").append(user).append(":").append(passwd)
                    .append("@").append(host).append(":").append(std::to_string(port))
                    .append("/").append(database);
            }
        };

        SessionInfo info = {"127.0.0.1", 33060, "lc", "123456lc", "lc_test" };
        LOGI("connect MySQL server: %s", info.get_uri().c_str());

        // mysqlx::Session sess(info.host, info.port, info.user, info.passwd, info.database);
        mysqlx::Session sess(info.get_uri());

        // 2. 查看 MySQL 版本
        mysqlx::SqlResult res = sess.sql("SELECT VERSION();").execute();
        mysqlx::Row row = res.fetchOne();
        if (row) {
            LOGI("MySQL Verison: %s", row[0].get<std::string>().c_str());
        }

        // 3.创建表单
        std::string table_name = "students";
        sess.sql("CREATE TABLE IF NOT EXISTS " + table_name + " (id INTEGER AUTO_INCREMENT PRIMARY KEY,"
                 "name VARCHAR(50) NOT NULL,"
                 "age INTEGER DEFAULT 0,"
                 "photo TEXT)")
                 .execute();

        // 4.插入表单
        for (const auto& stu : students) {
            sess.sql("INSERT INTO " + table_name + "(name, age, photo) VALUES (?, ?, ?)")
                .bind(stu.name, stu.age, stu.photo)
                .execute();
        }

        // 5.查询数据
        res = sess.sql("SELECT id, name, age, photo FROM " + table_name + " ORDER BY id").execute();

        for (const mysqlx::Row& row : res) {
            LOGI("id: %d, name: %s, age: %d, photo: %s", row[0].get<int>(),
                row[1].get<std::string>().c_str(),
                row[2].get<int>(),
                row[3].get<std::string>().c_str()
                );
        }

        // 6. 更新数据
        sess.sql("UPDATE " + table_name + " SET age=4 WHERE name='lsy'").execute();
        LOGI("update lsy age to 4...");

        res = sess.sql("SELECT id, name, age, photo FROM " + table_name + " ORDER BY id").execute();
        for (const mysqlx::Row& row : res) {
            LOGI("id: %d, name: %s, age: %d, photo: %s", row[0].get<int>(),
                 row[1].get<std::string>().c_str(),
                 row[2].get<int>(),
                 row[3].get<std::string>().c_str()
                 );
        }

        // 7. 删除所有数据
        sess.sql("DELETE FROM " + table_name).execute();

        // 8. 删除表
        sess.sql("DROP TABLE " + table_name).execute();

        sess.close();

    } catch (const mysqlx::Error& e) {
        std::cerr << "MySQL Error: " << e << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Std Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int opencv_test() {
    cv::Mat img = cv::imread("face1.jpg", cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cerr << "img is empty.." << std::endl;
        return -1;
    }
    return 0;
}

int eigen_test() {
    Eigen::Vector3d v3d = Eigen::Vector3d::Identity();

    std::cout << "v3d:" << v3d.transpose() << std::endl;

    Eigen::Vector3d res = v3d * 4;
    std::cout << "res:" << res.transpose() << std::endl;

    LOGI("res:%f, %f, %f", res(0), res(1), res(2));
    return 0;
}

int yamlcpp_test() {
    YAML::Node node;

    return 0;
}

int main() {
    LOGI("============== mysql::Main::Hello World \n\n");

    // LOGI("============== sqlite3_test start ====");
    // sqlite3_test();
    // LOGI("============== sqlite3_test end ====\n\n");

    // LOGI("============== mysql_connector_test start ====");
    // mysql_connector_test();
    // LOGI("============== mysql_connector_test end ====\n\n");

    // LOGI("============== opencv_test start ====");
    // opencv_test();
    // LOGI("============== opencv_test end ====\n\n");

    // LOGI("============== eigen_test start ====");
    // eigen_test();
    // LOGI("============== eigen_test end ====\n\n");

    return 0;
}




