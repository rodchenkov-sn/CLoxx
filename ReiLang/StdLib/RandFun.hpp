#pragma once
#include "../Callable.hpp"

class RandFun : public Callable
{
public:
    [[nodiscard]] unsigned arity() const override;
    Value call(Interpreter& interpreter, std::vector<Value> args) override;
    [[nodiscard]] std::string toString() const override;
};
