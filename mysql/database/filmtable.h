#ifndef FILMTABLE_H
#define FILMTABLE_H

#include "table.h"

class FilmTable : public Table
{
public:
    FilmTable(std::string name);
    virtual ~FilmTable();
};

#endif // FILMTABLE_H
