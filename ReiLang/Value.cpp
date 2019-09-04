#include "Value.hpp" // one more test
#include <sstream>
#include <iomanip>

#include "Callable.hpp"
#include "Instance.hpp"

const char* to_string(ValueType e)
{
    switch (e) {
    case ValueType::Nil      : return "Nil";
    case ValueType::Bool     : return "Bool";
    case ValueType::Number   : return "Number";
    case ValueType::String   : return "String";
    case ValueType::Callable : return "Callable";
    case ValueType::Instance : return "Instance";
    default : return "unknown";
    }
}

ValueOperationException::ValueOperationException(std::string msg):
    msg_(std::move(msg))
{
}

ValueOperationException::ValueOperationException(ValueType src, ValueType dst)
{
    msg_ = "Unable to cast \'" + std::string(to_string(src)) + "\' to \'" + std::string(to_string(dst)) + "\'.";
}

char const* ValueOperationException::what() const
{
    return msg_.c_str();
}

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

Value::Value(std::weak_ptr<Callable> value):
    type_(ValueType::Callable),
    value_(std::move(value))
{
}

Value::Value(std::weak_ptr<Instance> value):
    type_(ValueType::Instance),
    value_(std::move(value))
{
}

Value Value::operator-() const
{
    if (type_ == ValueType::Number) {
        return Value{ -1 * std::get<double>(value_) };
    }
    throw ValueOperationException{ type_, ValueType::Number };
}

Value Value::operator!() const
{
    return Value{ !isTrue() };
}

Value Value::operator+(const Value& rhs) const
{
    if (type_ == ValueType::Number && rhs.type_ == ValueType::Number) {
        return Value{ std::get<double>(value_) + std::get<double>(rhs.value_) };
    }
    if (type_ == ValueType::String || rhs.type_ == ValueType::String) {
        return Value{ toString() + rhs.toString() };
    }
    throw ValueOperationException{ rhs.type_, type_ };
}

Value Value::operator-(const Value& rhs) const
{
    if (type_ == ValueType::Number && rhs.type_ == ValueType::Number) {
        return Value{ std::get<double>(value_) - std::get<double>(rhs.value_) };
    }
    if (type_ == ValueType::Number) {
        throw ValueOperationException{ rhs.type_, ValueType::Number };
    }
    throw ValueOperationException{ type_, ValueType::Number };
}

Value Value::operator*(const Value& rhs) const
{
    if (type_ == ValueType::Number && rhs.type_ == ValueType::Number) {
        return Value{ std::get<double>(value_) * std::get<double>(rhs.value_) };
    }
    if (type_ == ValueType::Number) {
        throw ValueOperationException{ rhs.type_, ValueType::Number };
    }
    throw ValueOperationException{ type_, ValueType::Number };
}

Value Value::operator/(const Value& rhs) const
{
    if (type_ == ValueType::Number && rhs.type_ == ValueType::Number) {
        if (rhs.getNumber() == 0) {
            throw ValueOperationException{ "Zero division." };
        }
        return Value{ std::get<double>(value_) / std::get<double>(rhs.value_) };
    }
    if (type_ == ValueType::Number) {
        throw ValueOperationException{ rhs.type_, ValueType::Number };
    }
    throw ValueOperationException{ type_, ValueType::Number };
}

Value Value::operator==(const Value& rhs) const
{
    if (type_ != rhs.type_) {
        return Value{ false };
    }
    switch (type_) {
    case ValueType::Nil: 
        return Value{ true };
    case ValueType::Bool:
        return Value{ std::get<bool>(value_) == std::get<bool>(rhs.value_) };
    case ValueType::Number:
        return Value{ std::get<double>(value_) == std::get<double>(rhs.value_) };
    case ValueType::String:
        return Value{ std::get<std::string>(value_) == std::get<std::string>(rhs.value_) };
    default: ;
    }
    // unreachable
    return Value{ false };
}

Value Value::operator!=(const Value& rhs) const
{
    return !(*this == rhs);
}

Value Value::operator<(const Value& rhs) const
{
    if (type_ == ValueType::Number && rhs.type_ == ValueType::Number) {
        return Value{ std::get<double>(value_) < std::get<double>(rhs.value_) };
    }
    if (type_ == ValueType::Number) {
        throw ValueOperationException{ rhs.type_, ValueType::Number };
    }
    throw ValueOperationException{ type_, ValueType::Number };
}

Value Value::operator<=(const Value& rhs) const
{
    return *this < rhs || *this == rhs;
}

Value Value::operator>(const Value& rhs) const
{
    return !(*this <= rhs);
}

Value Value::operator>=(const Value& rhs) const
{
    return !(*this < rhs);
}

Value Value::operator||(const Value& rhs) const
{
    return Value{ isTrue() || rhs.isTrue() };
}

Value Value::operator&&(const Value& rhs) const
{
    return Value{ isTrue() && rhs.isTrue() };
}

ValueType Value::getType() const
{
    return type_;
}

bool Value::isTrue() const
{
    return type_ == ValueType::Bool ? getBool() : type_ != ValueType::Nil;
}

bool Value::getBool() const
{
    return std::get<bool>(value_);
}

double Value::getNumber() const
{
    return std::get<double>(value_);
}

std::shared_ptr<Callable> Value::getCallable() const
{
    return std::get<std::weak_ptr<Callable>>(value_).lock();
}

std::shared_ptr<Instance> Value::getInstance() const
{
    return std::get<std::weak_ptr<Instance>>(value_).lock();
}

std::string Value::getString() const
{
    return std::get<std::string>(value_);
}

std::string Value::toString() const
{
    switch (type_) {
    case ValueType::Nil: 
        return "nil";
    case ValueType::Bool: 
        return isTrue() ? "true" : "false";
    case ValueType::Number: {
        std::ostringstream strout;
        strout << std::fixed << std::setprecision(15) << std::get<double>(value_);
        std::string str = strout.str();
        size_t end = str.find_last_not_of('0') + 1;
        if (str[end - 1] == '.') end -= 1;
        return str.erase(end);
    }
    case ValueType::String:
        return std::get<std::string>(value_);
    case ValueType::Callable:
        return std::get<std::shared_ptr<Callable>>(value_)->toString();
    case ValueType::Instance:
        return std::get<std::shared_ptr<Instance>>(value_)->toString();
    default: ;
    }
    // unreachable
    return "";
}

std::string Value::toPrinter() const
{
    if (type_ == ValueType::String) {
        return "\"" + toString() + "\"";
    }
    return toString();
}
