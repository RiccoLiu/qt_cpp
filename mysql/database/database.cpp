#include <iostream>
#include <sstream>

#include "database.h"
#include "tablefactory.h"
#include "sqlfactory.h"
#include "logger.h"

class DatabaseImpl final : public Database {
public:
    DatabaseImpl();
    virtual ~DatabaseImpl();

    virtual std::vector<std::string> GetTableField() override {
        return table_->GetField();
    }

    virtual int Init(std::string db_file) override;
    virtual int GetUserCount() override ;
    virtual int AddUser(const UserInfo& user) override;
    virtual int DeleteUser(const UserInfo& user) override;
    virtual int UpdateUser(const UserInfo& where_user, const UserInfo& user) override;
    virtual std::vector<UserInfo> QueryUser() override;
    virtual std::vector<UserInfo> QueryUser(int page_size, int page) override;

    virtual void Test() override;

private:
    std::vector<SqlProcessor::RowData> create_data(std::shared_ptr<Table> table);
    std::vector<SqlProcessor::RowData> create_film_data(std::shared_ptr<FilmTable> table);
    std::vector<SqlProcessor::RowData> create_user_data(std::shared_ptr<UserTable> table);
    void print_data(std::shared_ptr<Table> table, const std::vector<SqlProcessor::RowData>& data);

private:
    UserInfo from_row(const SqlProcessor::RowData& row);
    SqlProcessor::RowData from_user(const UserInfo& user);

private:
    std::shared_ptr<Table> table_;
    std::shared_ptr<SqlProcessor> sql_;
};

Database* CreateDatabase() {
    return new DatabaseImpl();

}

void DestroyDatabase(Database* db) {
    if (db) {
        delete db;
    }
}

DatabaseImpl::DatabaseImpl()
    : table_(TableFactory::CreateTable(TableType::UserTable, "users"))
    , sql_( SqlFactory::CreateSql(SqlType::Sqlite))
{
}

DatabaseImpl::~DatabaseImpl()
{
}

int DatabaseImpl::Init(std::string db_file) {
    return sql_->InitTable(db_file, table_);
}

int DatabaseImpl::GetUserCount() {
    return sql_->GetRowCount(table_);
}

std::vector<UserInfo> DatabaseImpl::QueryUser() {
    std::vector<UserInfo> user_info;
    std::vector<SqlProcessor::RowData> data = sql_->QueryData(table_, SqlProcessor::RowData());
    for (const SqlProcessor::RowData& row : data) {
        user_info.push_back(from_row(row));
    }
    return user_info;
}

std::vector<UserInfo> DatabaseImpl::QueryUser(int page_size, int page) {
    std::vector<UserInfo> user_info;
    std::vector<SqlProcessor::RowData> data = sql_->QueryData(table_, SqlProcessor::RowData(), page_size, (page - 1) * page_size);
    for (const SqlProcessor::RowData& row : data) {
        user_info.push_back(from_row(row));
    }
    return user_info;
}

int DatabaseImpl::AddUser(const UserInfo& user) {
    sql_->InsertData(table_, from_user(user));
    return 0;
}

int DatabaseImpl::DeleteUser(const UserInfo& user) {
    sql_->DeleteData(table_, from_user(user));
    return 0;
}

int DatabaseImpl::UpdateUser(const UserInfo& where_user, const UserInfo& user) {
    sql_->UpdateData(table_, from_user(where_user), from_user(user));
    return 0;
}

UserInfo DatabaseImpl::from_row(const SqlProcessor::RowData& row) {
    std::string name;
    std::string gender;
    int age;

    std::string address;
    std::vector<double> keypoint;
    std::string image_path;

    for (auto iter = row.begin(); iter != row.end(); iter++) {
        std::string field = iter->first;
        FieldValue value = iter->second;

        if (field == table_->GetField(0)) {
            name = std::get<std::string>(value);
        }
        else if (field == table_->GetField(1)) {
            gender = std::get<std::string>(value);
        }
        else if (field == table_->GetField(2)) {
            age = std::get<int64_t>(value);
        }
        else if (field == table_->GetField(3)) {
            address = std::get<std::string>(value);
        }
        else if (field == table_->GetField(4)) {
            keypoint = std::get<std::vector<double>>(value);
        }
        else if (field == table_->GetField(5)) {
            image_path = std::get<std::string>(value);
        }
    }
    return {name, gender, age, address, keypoint, image_path};
}

SqlProcessor::RowData DatabaseImpl::from_user(const UserInfo& user) {
    SqlProcessor::RowData row;
    if (user.name.size() > 0) {
        row[table_->GetField(0)] = user.name;
    }
    if (user.gender.size() > 0) {
        row[table_->GetField(1)] = user.gender;
    }
    row[table_->GetField(2)] = user.age;
    if (user.address.size() > 0) {
        row[table_->GetField(3)] = user.address;
    }
    if (user.keypoint.size() > 0) {
        row[table_->GetField(4)] = user.keypoint;
    }
    if (user.image.size() > 0) {
        row[table_->GetField(5)] = user.image;
    }
    return row;
}

void DatabaseImpl::Test() {
    LOGI("DatabaseImpl::Test:: Hello World --\n");
    std::cout << "--------- Database Test Start -----" << std::endl;

    std::shared_ptr<Table> film_table = TableFactory::CreateTable(TableType::FilmTable, "films");
    std::shared_ptr<SqlProcessor> sql = SqlFactory::CreateSql(SqlType::Sqlite);

    sql->InitTable("film.db", film_table);

    std::vector<SqlProcessor::RowData> datas = create_data(film_table);
    for (const auto& data : datas) {
        sql->InsertData(film_table, data);
    }

    {
        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);
    }
    {
        std::cout << "---- delete " << film_table->GetField(0) << " = " << "weld_3" << std::endl;

        // 删除 weld_3
        SqlProcessor::RowData delete_row = {
            std::make_pair(film_table->GetField(0), "weld_3")
        };

        sql->DeleteData(film_table, delete_row);

        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);
    }
    {
        std::cout << "---- delete " << film_table->GetField(0) << " = " << "weld_5" << std::endl;

        // 删除 weld_5
        SqlProcessor::RowData delete_row = {
                std::make_pair(film_table->GetField(0), "weld_5")
        };
        sql->DeleteData(film_table, delete_row);

        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);

    }
    {
        std::cout << "---- delete " << film_table->GetField(0) << " = " << "weld_8" << std::endl;

        // 删除 weld_8
        SqlProcessor::RowData delete_row = {
            std::make_pair(film_table->GetField(0), "weld_8")
        };
        sql->DeleteData(film_table, delete_row);

        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);
    }
    {
        std::cout << "---- update data: " << film_table->GetField(0) << ": weld_4_new"
                  << " where:" << film_table->GetField(3) << ": /home/weld_4.dcm" << std::endl;

        SqlProcessor::RowData update_row = {
            std::make_pair(film_table->GetField(0), "weld_4_new")
        };
        SqlProcessor::RowData where_row = {
            std::make_pair(film_table->GetField(3), "/home/weld_4.dcm")
        };
        sql->UpdateData(film_table, where_row, update_row);


        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);
    }
    {

        std::cout << "---- update data: " << film_table->GetField(3) <<  ":/home/weld1.dcm"
                  << " where:" << film_table->GetField(0) << ": weld_1" << std::endl;;

        SqlProcessor::RowData update_row = {
            std::make_pair(film_table->GetField(3), "/home/weld_1_new.dcm")
        };
        SqlProcessor::RowData where_row = {
            std::make_pair(film_table->GetField(0), "weld_1")
        };
        sql->UpdateData(film_table, where_row, update_row);


        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);
    }
    {
        // 全部删除
        sql->DeleteData(film_table, SqlProcessor::RowData());

        std::vector<SqlProcessor::RowData> data = sql->QueryData(film_table, SqlProcessor::RowData());
        std::cout << "--- query data ---" << std::endl;
        print_data(film_table, data);
    }

    std::cout << "--------- Database Test End -----" << std::endl;
}


void DatabaseImpl::print_data(std::shared_ptr<Table> table, const std::vector<SqlProcessor::RowData>& data) {

    std::stringstream stream;

    for (int i = 0; i < data.size(); i++) {
        std::vector<std::string> fields = table->GetField();
        for (const auto& field : fields) {
            auto iter = data[i].find(field);
            if (iter != data[i].end()) {
                FieldValue value = iter->second;

               stream << field << ": ";
                if (std::holds_alternative<std::nullptr_t>(value)) {
                }
                else if (std::holds_alternative<int64_t>(value)) {
                    stream << std::get<int64_t>(value);
                }
                else if (std::holds_alternative<double>(value)) {
                    stream << std::get<double>(value);
                }
                else if (std::holds_alternative<std::string>(value)) {
                    stream << std::get<std::string>(value);
                }
                else if (std::holds_alternative<std::vector<double>>(value)) {
                }
                stream << "\t";
            }
            else {
                stream << field << ": \t\t";
            }
        }
        stream << std::endl;
    }
    LOGI("%s", stream.str().c_str());
}

std::vector<SqlProcessor::RowData> DatabaseImpl::create_film_data(std::shared_ptr<FilmTable> table) {
    static std::vector<SqlProcessor::RowData> data  = {
        {
            std::make_pair(table->GetField(0), "weld_0"),
            std::make_pair(table->GetField(1), 1.0),
            std::make_pair(table->GetField(2), 2.0),
            std::make_pair(table->GetField(3), "/home/weld_0.dcm")
        },
        {
            std::make_pair(table->GetField(0), "weld_1"),
            std::make_pair(table->GetField(1), 2.0),
            std::make_pair(table->GetField(2), 4.0)
        },
        {
            std::make_pair(table->GetField(0), "weld_2"),
            std::make_pair(table->GetField(1), 4.0),
            std::make_pair(table->GetField(3), "/home/weld_2.dcm")
        },
        {
            std::make_pair(table->GetField(0), "weld_3"),
            std::make_pair(table->GetField(2), 8.0),
            std::make_pair(table->GetField(3), "/home/weld_3.dcm")
        },
        {
            std::make_pair(table->GetField(1), 8),
            std::make_pair(table->GetField(2), 16.0),
            std::make_pair(table->GetField(3), "/home/weld_4.dcm")
        },
        {
            std::make_pair(table->GetField(0), "weld_5"),
            std::make_pair(table->GetField(1), 16),
            std::make_pair(table->GetField(2), 32.0),
            std::make_pair(table->GetField(3), "/home/weld_5.dcm")
        }
    };
    return data;
}

std::vector<SqlProcessor::RowData> DatabaseImpl::create_user_data(std::shared_ptr<UserTable> table) {
    std::vector<SqlProcessor::RowData> data;
    return data;
}

std::vector<SqlProcessor::RowData> DatabaseImpl::create_data(std::shared_ptr<Table> table) {
    std::vector<SqlProcessor::RowData> data;
    std::shared_ptr<FilmTable> film_table = std::dynamic_pointer_cast<FilmTable>(table);
    if (film_table) {
        return create_film_data(film_table);
    }

    std::shared_ptr<UserTable> user_table = std::dynamic_pointer_cast<UserTable>(table);
    if (user_table) {
        return create_user_data(user_table);
    }
    return data;
}


