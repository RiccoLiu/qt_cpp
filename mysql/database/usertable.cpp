#include <iostream>
#include <map>

#include "usertable.h"

// std::map 是有序容器，会自动按照Key进行排序，所以插入顺序和声明先后顺序无光
// static const std::map<std::string, FieldType> kUserTable = {
//     std::make_pair("id", FieldType::INTEGER),
//     std::make_pair("name", FieldType::TEXT),
//     std::make_pair("gender", FieldType::CHAR),
//     std::make_pair("age", FieldType::INTEGER),
//     std::make_pair("address", FieldType::VARCHAR),
//     std::make_pair("feature", FieldType::BLOB),
//     std::make_pair("image_path", FieldType::TEXT)
// };

static const std::vector<std::pair<std::string, FieldType>> kUserTable = {
    std::make_pair("name", FieldType::TEXT),
    std::make_pair("gender", FieldType::CHAR),
    std::make_pair("age", FieldType::INTEGER),
    std::make_pair("address", FieldType::VARCHAR),
    std::make_pair("feature", FieldType::BLOB),
    std::make_pair("image_path", FieldType::TEXT)
};

UserTable::UserTable(std::string name)
    : Table(name) {
    for (auto it = kUserTable.begin(); it != kUserTable.end(); it++) {
        field_.push_back(it->first);
        type_.push_back(it->second);
    }
}

UserTable::~UserTable() {
}
