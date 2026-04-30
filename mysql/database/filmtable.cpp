#include "filmtable.h"

// std::map 是有序容器，会自动按照Key进行排序，所以插入顺序和声明先后顺序无光
// static const std::map<std::string, FieldType> kFilmTable = {
//     std::make_pair("WeldName", FieldType::TEXT),
//     std::make_pair("WallThickness", FieldType::DOUBLE),
//     std::make_pair("PipeDiameter", FieldType::DOUBLE),
//     std::make_pair("path", FieldType::TEXT)
// }

static const std::vector<std::pair<std::string, FieldType>> kFilmTable = {
    std::make_pair("weld_name", FieldType::TEXT),
    std::make_pair("wall_thickness", FieldType::DOUBLE),
    std::make_pair("pipe_diameter", FieldType::DOUBLE),
    std::make_pair("path", FieldType::TEXT)
};


FilmTable::FilmTable(std::string name)
    : Table(name) {
    for (auto it = kFilmTable.begin(); it != kFilmTable.end(); it++) {
        field_.push_back(it->first);
        type_.push_back(it->second);
    }
}

FilmTable::~FilmTable() {
}

