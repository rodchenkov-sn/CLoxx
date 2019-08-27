#include "Function.hpp"
#include "Interpreter.hpp"

Function::Function(const std::shared_ptr<Stmt::Function>& declaration) :
    params_(declaration->params()),
    body_(declaration->body()),
    name_(declaration->name().lexeme)
{
}

Function::Function(const std::shared_ptr<Expr::Lambda>& declaration):
    params_(declaration->params()),
    body_(declaration->body()),
    name_("Lambda")
{
}

unsigned Function::arity() const
{
    return params_.size();
}

Value Function::call(Interpreter& interpreter, std::vector<Value> args)
{
    auto environment = std::make_shared<Environment>( interpreter.global_.get() );
    for (size_t i = 0; i < args.size(); i++) {
        auto name = params_[i].lexeme;
        auto val = args[i];
        environment->define(name, val);
    }
    interpreter.execute_block_(body_, environment);
    return Value{};
}

std::string Function::toString() const
{
    return name_ + " :: t -> t1";
}
