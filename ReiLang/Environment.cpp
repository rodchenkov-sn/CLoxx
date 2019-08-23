#include "Environment.hpp"

EnvironmentException::EnvironmentException(const std::string& name)
{
    msg_ = "Undefined variable \'" + name + "\'.";
}

char const* EnvironmentException::what() const
{
    return msg_.c_str();
}

void Environment::define(const std::string& name, const Value& value)
{
    values_[name] = value;
}

void Environment::assign(const std::string& name, const Value& value)
{
    if (values_.find(name) == values_.end()) {
        throw EnvironmentException{ name };
    }
    values_[name] = value;
}

Value Environment::lookup(const std::string& name)
{
    if (values_.find(name) == values_.end()) {
        throw EnvironmentException{ name };
    }
    return values_[name];
}
