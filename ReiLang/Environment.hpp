#pragma once
#include <map>
#include <string>
#include "Value.hpp"

class EnvironmentException final : std::exception
{
public:
    explicit EnvironmentException(const std::string& name);
    [[nodiscard]] char const* what() const override;
private:
    std::string msg_;
};

class Environment
{
public:
    Environment();
    explicit Environment(Environment* enclosing);
    void define(const std::string& name, const Value& value);
    void assign(const std::string& name, const Value& value);
    Value lookup(const std::string& name);
private:
    std::map<std::string, Value> values_;
    Environment* enclosing_;
};

