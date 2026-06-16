#ifndef MYSQL_DATABASE_H
#define MYSQL_DATABASE_H

#include <mysqlx/xdevapi.h>

#include "database.h"

class MysqlDatabase final : public Database {
public:
    MysqlDatabase(std::unique_ptr<mysqlx::Session> sess, mysqlx::Schema schema);
    virtual ~MysqlDatabase();

    virtual std::optional<int> AddUser(const User& user) override;
    virtual size_t DeleteUser(const UserCondition& condition) override;
    virtual size_t UpdateUser(const UserCondition& where, const UserCondition& values) override;
    virtual std::vector<User> QueryUser(const UserCondition& condition, size_t page = 0, size_t page_size = 20) override;
    virtual std::vector<User> GetAllUsers(int limit = -1) override;
    virtual size_t GetUserCount() override;
    virtual int Test() override;

    int CreateTableIfNotExists();

private:
    User CreateUser(const mysqlx::Row& row, const std::map<std::string, size_t>& col_lable_to_idx);

private:
    mysqlx::Schema schema_;
    std::unique_ptr<mysqlx::Session> sess_;

    std::string table_name_;
};

#endif // MYSQL_DATABASE_H
