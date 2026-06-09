#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <string>
#include "database2.h"

Gender ToGender(const std::string& str);
std::string ToString(const Gender& gender);

std::string ToString(const UserKey& key);
std::string ToString(const UserValue& value);
std::string ToString(const UserKey& key, const UserValue& value);

UserCondition ToCondition(const User& user);

// template<typename T>
// std::string SerializeVectorAsBlob(const std::vector<T>& vec);

template<typename T>
std::string SerializeVectorAsBlob(const std::vector<T>& vec) {
    // 确保 T 是平凡可复制的（POD-like），避免对 string、vector 等复杂类型误用
    static_assert(
        std::is_trivially_copyable_v<T>,
        "to_blob only supports trivially copyable types (e.g., int, float, double)"
        );

    if (vec.empty()) {
        return {};
    }

    return std::string(
        reinterpret_cast<const char*>(vec.data()),
        vec.size() * sizeof(T)
        );
}

#endif // DB_UTILS_H
