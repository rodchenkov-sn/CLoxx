#pragma once

#include "Callable.hpp"
#include "Ast.hpp"

class Function : public Callable
{
public:
    explicit Function(std::shared_ptr<Stmt::Function> declaration);
    [[nodiscard]] unsigned arity() const override;
    Value call(Interpreter& interpreter, std::vector<Value> args) override;
    [[nodiscard]] std::string toString() const override;
private:
    std::shared_ptr<Stmt::Function> declaration_;
};

