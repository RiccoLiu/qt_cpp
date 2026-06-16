
#include "db_utils.h"

Gender ToGender(const std::string& str) {
    return (str == "Male") ? Gender::Male : Gender::Female;
}

std::string ToString(const Gender& gender) {
    return (gender == Gender::Male) ? "Male" : "Female";
}

std::string ToString(const UserKey& key) {
    std::string str;
    if (key == UserKey::Id) {
        str = "user_id";
    } else if (key == UserKey::Name) {
        str = "name";
    } else if (key == UserKey::Gender) {
        str = "gender";
    } else if (key == UserKey::Age) {
        str = "age";
    } else if (key == UserKey::ImagePath) {
        str = "image_path";
    } else if (key == UserKey::Keypoint) {
        str = "keypoint";
    } else {
    }
    return str;
}

std::string ToString(const UserValue& value) {
    std::string str;
    if (std::holds_alternative<int>(value)) {
        int value_act = std::get<int>(value);
        str = std::to_string(value_act);
    } else if (std::holds_alternative<Gender>(value)) {
        Gender value_act = std::get<Gender>(value);
        str = ToString(value_act);
    } else if (std::holds_alternative<std::string>(value)) {
        std::string value_act = std::get<std::string>(value);
        str = value_act;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::vector<double> value_act = std::get<std::vector<double>>(value);
        str = ToString(value_act);
    }
    return str;
}

std::string ToString(const UserKey& key, const UserValue& value) {
    std::string str;
    str.append(ToString(key)).append("=").append(ToString(value));
    return str;
}

UserCondition ToCondition(const User& user) {
    UserCondition condition;
    condition[UserKey::Id] = user.id;
    condition[UserKey::Name] = user.name;
    condition[UserKey::Gender] = user.gender;
    condition[UserKey::Age] = user.age;
    condition[UserKey::ImagePath] = user.image_path;
    condition[UserKey::Keypoint] = user.keypoint;
    return condition;
}

// template<typename T>
// std::string SerializeVectorAsBlob(const std::vector<T>& vec) {
//     // 确保 T 是平凡可复制的（POD-like），避免对 string、vector 等复杂类型误用
//     static_assert(
//         std::is_trivially_copyable_v<T>,
//         "to_blob only supports trivially copyable types (e.g., int, float, double)"
//         );

//     if (vec.empty()) {
//         return {};
//     }

//     return std::string(
//         reinterpret_cast<const char*>(vec.data()),
//         vec.size() * sizeof(T)
//         );
// }

// template std::string SerializeVectorAsBlob<double>(const std::vector<double>&);
