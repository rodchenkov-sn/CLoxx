#pragma once
#include "Value.hpp"
#include <vector>

class Interpreter;

class Callable
{
public:
    virtual ~Callable() = default;
    [[nodiscard]] virtual unsigned int arity() const = 0;
    virtual Value call(Interpreter& interpreter, std::vector<Value> args) = 0;
    [[nodiscard]] virtual std::string toString() const = 0;
};
