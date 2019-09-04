#pragma once

#include "Callable.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

class Function : public Callable
{
public:
    explicit Function(Stmt::Function* declaration, std::shared_ptr<Environment> closure);
    explicit Function(Expr::Lambda* declaration, std::shared_ptr<Environment> closure);
    [[nodiscard]] unsigned arity() const override;
    Value call(Interpreter& interpreter, std::vector<Value> args) override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::shared_ptr<Function> bind(const std::weak_ptr<Instance>& instance) const;
private:
    std::vector<Token> params_;
    std::list<Stmt::Base::Ptr> body_;
    std::string name_;
    std::shared_ptr<Environment> closure_;
	Stmt::Function* declaration_;
};

