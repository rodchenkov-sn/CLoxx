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
    void assignAt(const std::string& name, const Value& value, unsigned distance);
    [[nodiscard]] Value lookup(const std::string& name) const;
    [[nodiscard]] Value lookupAt(const std::string& name, unsigned distance);
private:
    [[nodiscard]] Environment* ancestor_(unsigned int distance);

    std::map<std::string, Value> values_;
    Environment* enclosing_;
};

