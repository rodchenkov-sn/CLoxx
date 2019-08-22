#include "Value.hpp"

Value::Value():
    type_(ValueType::Nil)
{
}

Value::Value(const bool value):
    type_(ValueType::Bool),
    value_(value)
{
}

Value::Value(const double value):
    type_(ValueType::Number),
    value_(value)
{
}

Value::Value(std::string value):
    type_(ValueType::String),
    value_(std::move(value))
{
}

ValueType Value::getType() const
{
    return type_;
}

bool Value::getBool() const
{
    return std::get<bool>(value_);
}

double Value::getNumber() const
{
    return std::get<double>(value_);
}

std::string Value::getString() const
{
    return std::get<std::string>(value_);
}
