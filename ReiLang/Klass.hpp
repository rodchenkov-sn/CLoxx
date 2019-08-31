#pragma once
#include <string>
#include "Callable.hpp"

class Klass : public Callable
{
public:
    explicit Klass(std::string name);
    [[nodiscard]] std::string toString() const override;
    Value call(Interpreter& interpreter, std::vector<Value> args) override;
    [[nodiscard]] unsigned arity() const override;
private:
    std::string name_;
};

