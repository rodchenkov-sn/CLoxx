#pragma once

#include "Callable.hpp"
#include "Ast.hpp"

class Function : public Callable
{
public:
    explicit Function(const std::shared_ptr<Stmt::Function>& declaration);
    explicit Function(const std::shared_ptr<Expr::Lambda>& declaration);
    [[nodiscard]] unsigned arity() const override;
    Value call(Interpreter& interpreter, std::vector<Value> args) override;
    [[nodiscard]] std::string toString() const override;
private:
    std::vector<Token> params_;
    std::list<Stmt::Base::Ptr> body_;
    std::string name_;
};

