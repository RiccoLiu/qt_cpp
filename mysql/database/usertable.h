#ifndef USERTABLE_H
#define USERTABLE_H

#include "table.h"

class UserTable : public Table
{
public:
    UserTable(std::string name);
    virtual ~UserTable();

};

#endif // USERTABLE_H
