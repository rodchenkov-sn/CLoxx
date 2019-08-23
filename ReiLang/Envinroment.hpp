#pragma once
#include <map>
#include <string>
#include "Value.hpp"
#include "Token.hpp"

class Environment
{
public:
    Environment() = default;
    void define(const std::string& name, const Value& value);
    Value lookup(Token name);
private:
    std::map<std::string, Value> values_;
};
