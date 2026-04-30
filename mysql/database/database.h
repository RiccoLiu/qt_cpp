#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

struct UserInfo {
    std::string name;
    std::string gender;
    int age;
    std::string address;
    std::vector<double> keypoint;
    std::string image;
};

class Database {
public:
    Database() {}
    virtual ~Database() {}

    virtual int Init(std::string db_file) = 0;
    virtual std::vector<std::string> GetTableField() = 0;
    virtual int GetUserCount() = 0;
    virtual int AddUser(const UserInfo& user) = 0;
    virtual int DeleteUser(const UserInfo& user) = 0;
    virtual int UpdateUser(const UserInfo& where_user, const UserInfo& user) = 0;
    virtual std::vector<UserInfo> QueryUser() = 0;
    virtual std::vector<UserInfo> QueryUser(int page_size, int page) = 0;

    virtual void Test() = 0;
};

Database* CreateDatabase();
void DestroyDatabase(Database* db);

#endif // DATABASE_H
