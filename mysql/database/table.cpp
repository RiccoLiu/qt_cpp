
#include <iostream>

#include "table.h"

Table::Table(const std::string name)
    : name_(name) {
}

Table::~Table() {
}

std::string Table::GetField(int column) const {
    std::string field;
    if (column < field_.size()) {
        field = field_.at(column);
    }
    return field;
}

FieldType Table::GetType(int column) const {
    FieldType type = FieldType::INVALID;
    if (column < field_.size()) {
        type = type_.at(column);
    }
    return type;
}

FieldType Table::GetType(const std::string& field) const {
    int index = -1;
    for (int i = 0; i < field_.size(); i++) {
        if (field_[i] == field) {
            index = i;
            break;
        }
    }
    FieldType type = FieldType::INVALID;
    if (index > 0) {
        type = type_[index];
    }
    return type;
}

const std::string Table::GetString(FieldType type) {
    const std::map<FieldType, std::string> field_to_string = {
        std::make_pair(FieldType::INTEGER, "INTEGER"),
        std::make_pair(FieldType::CHAR, "CHAR(64)"),
        std::make_pair(FieldType::VARCHAR, "VARCHAR(256)"),
        std::make_pair(FieldType::FLOAT, "FLOAT"),
        std::make_pair(FieldType::DOUBLE, "DOUBLE"),
        std::make_pair(FieldType::TEXT, "TEXT"),
        std::make_pair(FieldType::BLOB, "BLOB")
    };
    std::string str = "";
    auto find = field_to_string.find(type);
    if (find != field_to_string.end()) {
        str = find->second;
    }
    return str;
}

int Table::PrintInfo() const {
    std::cout << "---- table name: " << name_ << "----" << std::endl;
    for (int idx = 0; idx < field_.size(); idx++) {
        std::cout << "idx:" << idx << ", "
                  << "(" << field_[idx] << " "
                  <<  GetString(type_[idx]) << ")"
                  << std::endl;
    }
    return 0;
}
