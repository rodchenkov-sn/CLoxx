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

void Environment::assignAt(const std::string& name, const Value& value, const unsigned distance)
{
    ancestor_(distance)->values_.at(name) = value;
}

Value Environment::lookup(const std::string& name) const
{
    if (values_.find(name) == values_.end()) {
        if (enclosing_) {
            return enclosing_->lookup(name);
        }
        throw EnvironmentException{ name };
    }
    return values_.at(name);
}

Value Environment::lookupAt(const std::string& name, unsigned distance)
{
    auto ancestor = ancestor_(distance);
    if (ancestor->values_.find(name) != ancestor->values_.end()) {
        return ancestor->values_.at(name);
    }
    throw EnvironmentException{ name };
}

Environment* Environment::ancestor_(const unsigned distance)
{
    auto* ancestor = const_cast<Environment*>(this);
    for (unsigned i = 0; i < distance; i++) {
        ancestor = ancestor->enclosing_;
    }
    return ancestor;
}
