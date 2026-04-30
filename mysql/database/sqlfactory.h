#ifndef SQLFACTORY_H
#define SQLFACTORY_H

#include <memory>

#include "sqlprocessor.h"
#include "sqliteprocessor.h"

enum class SqlType {
    Sqlite,
    MySQL
};

class SqlFactory {
public:
    SqlFactory() {}
    virtual ~SqlFactory() {}

    static std::shared_ptr<SqlProcessor> CreateSql(SqlType type) {
        if (type == SqlType::Sqlite) {
            return std::make_shared<SqliteProcessor>();
        }
        else {
            return nullptr;
        }
    }
};

#endif // SQLFACTORY_H
