#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <map>
#include <vector>
#include <variant>

enum class FieldType {
    INVALID,
    INTEGER,
    CHAR,
    VARCHAR,
    FLOAT,
    DOUBLE,
    TEXT,
    BLOB
};

using FieldValue = std::variant<
    std::nullptr_t,         // NULL
    int64_t,                // INTEGER
    double,                 // REAL (FLOAT + DOUBLE)
    std::string,            // TEXT / CHAR / VARCHAR
    std::vector<double>    // BLOB
    >;

class Table {
public:
    Table(const std::string name);
    virtual ~Table();

    virtual const std::string GetName() const {
        return name_;
    }
    virtual const size_t GetFieldSize() const {
        return field_.size();
    }
    virtual const std::vector<std::string> GetField() const {
        return field_;
    }
    virtual const std::vector<FieldType> GetType() const {
        return type_;
    }

    virtual std::string GetField(int column) const;
    virtual FieldType GetType(int column) const;
    virtual FieldType GetType(const std::string& field) const;

    static const std::string GetString(FieldType type);

    virtual int PrintInfo() const;

protected:
    std::string name_;
    std::vector<std::string> field_;
    std::vector<FieldType> type_;
};


#endif // TABLE_H
