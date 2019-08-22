#pragma once

#include <variant>
#include <string>

enum class ValueType
{
    Nil,
    Bool,
    Number,
    String
};

class Value
{
public:
    Value();
    explicit Value(bool value);
    explicit Value(double value);
    explicit Value(std::string value);

    [[nodiscard]] ValueType getType() const;

    [[nodiscard]] bool        getBool()   const;
    [[nodiscard]] double      getNumber() const;
    [[nodiscard]] std::string getString() const;

private:
    ValueType type_;
    std::variant<bool, double, std::string> value_;
};
