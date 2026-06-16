
#include <iostream>
#include <logger2.h>
#include <memory>

#include "database.h"
#include "mysql_database.h"
#include "sqlite_database.h"

std::unique_ptr<Database> DatabaseFactory::Create(const MySqlConfig& config) {
    try {
        std::unique_ptr<mysqlx::Session> sess = std::make_unique<mysqlx::Session>(
            config.host,
            config.port,
            config.username,
            config.passwd,
            config.schema);
        if (sess) {
            return std::make_unique<MysqlDatabase>(std::move(sess), sess->getSchema(config.schema));
        }
    } catch (const mysqlx::Error& e) {
        std::cerr << "MySQL Error: " << e << std::endl;
        return nullptr;
    } catch (const std::exception& e) {
        std::cerr << "Std Exception: " << e.what() << std::endl;
        return nullptr;
    }
    return nullptr;
}

std::unique_ptr<Database> DatabaseFactory::Create(const SqliteConfig& config) {
    return std::make_unique<SqliteDatabase>(config.db_file);
}
