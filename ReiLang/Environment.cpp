#include "Environment.hpp"

#include "Logger.hpp"
#include <iostream>

Logger logger{ std::cout };

EnvironmentException::EnvironmentException(const std::string& name)
{
    msg_ = "Undefined variable \'" + name + "\'.";
}

char const* EnvironmentException::what() const
{
    return msg_.c_str();
}

Environment::Environment():
    enclosing_(nullptr)
{
}

Environment::Environment(Environment* enclosing):
    enclosing_(enclosing)
{
}

void Environment::define(const std::string& name, const Value& value)
{
    logger.log(LogLevel::Debug, "defining var " + name + " with val " + value.toString());
    values_[name] = value;
}

void Environment::assign(const std::string& name, const Value& value)
{
    logger.log(LogLevel::Debug, "Trying to assign var " + name + " with val " + value.toString());
    if (values_.find(name) == values_.end()) {
        if (enclosing_) {
            enclosing_->assign(name, value);
            return;
        }
        throw EnvironmentException{ name };
    }
    logger.log(LogLevel::Debug, "Assigning var " + name + " with val " + value.toString());
    values_[name] = value;
}

Value Environment::lookup(const std::string& name)
{
    if (values_.find(name) == values_.end()) {
        if (enclosing_) {
            return enclosing_->lookup(name);
        }
        throw EnvironmentException{ name };
    }
    return values_[name];
}
