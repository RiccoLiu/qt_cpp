#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <optional>
#include <memory>

enum class Gender {
    Male,
    Female
};

struct User {
    int id;
    std::string name;
    Gender gender;
    int age;

    std::string image_path;
    std::vector<double> keypoint;
};

enum class UserKey {
    Id,         // int
    Age,        // int
    Gender,     // std::string
    Name,       // std::string
    ImagePath,  // std::string
    Keypoint    // std::vector<double>
};

using UserValue = std::variant<int, Gender, std::string, std::vector<double>>;
using UserCondition = std::map<UserKey, UserValue>;

class Database {
public:
    virtual ~Database() = default;

    virtual std::optional<int> AddUser(const User& user) = 0;
    virtual size_t DeleteUser(const UserCondition& condition) = 0;
    virtual size_t UpdateUser(const UserCondition& where, const UserCondition& values) = 0;
    virtual std::vector<User> QueryUser(const UserCondition& condition, size_t page = 0, size_t page_size = 20) = 0;
    virtual std::vector<User> GetAllUsers(int limit = -1) = 0;
    virtual size_t GetUserCount() = 0;
    virtual int Test() = 0;
};

struct MySqlConfig {
    std::string host = "127.0.0.1";
    int port = 33060;

    std::string username;
    std::string passwd;
    std::string schema; // 数据库名
};

struct SqliteConfig {
    std::string db_file;
};

class DatabaseFactory {
public:
    static std::unique_ptr<Database> Create(const MySqlConfig& config);
    static std::unique_ptr<Database> Create(const SqliteConfig& config);
};

#endif // DATABASE_H
