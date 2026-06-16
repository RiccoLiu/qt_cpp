#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include <sqlite3.h>

#include "database.h"

class SqliteDatabase final : public Database {
public:
    SqliteDatabase(std::string db_file);
    virtual ~SqliteDatabase();

    virtual std::optional<int> AddUser(const User& user) override ;
    virtual size_t DeleteUser(const UserCondition& condition) override;
    virtual size_t UpdateUser(const UserCondition& where, const UserCondition& values) override;
    virtual std::vector<User> QueryUser(const UserCondition& condition, size_t page = 0, size_t page_size = 20) override;
    virtual std::vector<User> GetAllUsers(int limit = -1) override;
    virtual size_t GetUserCount() override;
    virtual int Test() override;

    int CreateTableIfNotExists();

private:
    User CreateUser(sqlite3_stmt* stmt);

private:
    sqlite3* sqlite_;
    std::string db_file_;
    std::string table_name_;
};

#endif // SQLITE_DATABASE_H
