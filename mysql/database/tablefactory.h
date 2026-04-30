#ifndef TABLEFACTORY_H
#define TABLEFACTORY_H

#include <memory>

#include "usertable.h"
#include "filmtable.h"

enum class TableType {
    UserTable,
    FilmTable
};

class TableFactory {
public:
    TableFactory() {}
    virtual ~TableFactory() {}

    static std::shared_ptr<Table> CreateTable(TableType type, std::string table_name) {
        if (type == TableType::UserTable) {
            return std::make_shared<UserTable>(table_name);
        }
        else if (type == TableType::FilmTable) {
            return std::make_shared<FilmTable>(table_name);
        }
        else {
            return nullptr;
        }
    }
};



#endif // TABLEFACTORY_H
