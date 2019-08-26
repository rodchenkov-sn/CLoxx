#pragma once

#include <variant>
#include <string>
#include <memory>

class Callable;

enum class ValueType
{
    Nil,
    Bool,
    Number,
    String,
    Callable
};

const char* to_string(ValueType e);

class ValueOperationException final : std::exception
{
public:
    explicit ValueOperationException(std::string msg);
    ValueOperationException(ValueType src, ValueType dst);
    [[nodiscard]] char const* what() const override;
private:
    std::string msg_;
};

class Value
{
public:
    Value();
    explicit Value(bool value);
    explicit Value(double value);
    explicit Value(std::string value);
    explicit Value(std::shared_ptr<Callable> value);

    Value operator -  ()                 const;
    Value operator !  ()                 const;
    Value operator +  (const Value& rhs) const;
    Value operator -  (const Value& rhs) const;
    Value operator *  (const Value& rhs) const;
    Value operator /  (const Value& rhs) const;
    Value operator == (const Value& rhs) const;
    Value operator != (const Value& rhs) const;
    Value operator <  (const Value& rhs) const;
    Value operator <= (const Value& rhs) const;
    Value operator >  (const Value& rhs) const;
    Value operator >= (const Value& rhs) const;
    Value operator || (const Value& rhs) const;
    Value operator && (const Value& rhs) const;

    [[nodiscard]] ValueType                 getType()     const;
    [[nodiscard]] bool                      isTrue()      const;
    [[nodiscard]] bool                      getBool()     const;
    [[nodiscard]] double                    getNumber()   const;
    [[nodiscard]] std::shared_ptr<Callable> getCallable() const;
    [[nodiscard]] std::string               getString()   const;
    [[nodiscard]] std::string               toString()    const;
    [[nodiscard]] std::string               toPrinter()   const;

private:
    ValueType type_;
    std::variant<bool, double, std::string, std::shared_ptr<Callable>> value_;
};
