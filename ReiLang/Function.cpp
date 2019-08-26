#include "Function.hpp"
#include "Interpreter.hpp"

Function::Function(std::shared_ptr<Stmt::Function> declaration):
    declaration_(std::move(declaration))
{
}

unsigned Function::arity() const
{
    return declaration_->params().size();
}

Value Function::call(Interpreter& interpreter, std::vector<Value> args)
{
    auto environment = std::make_shared<Environment>( interpreter.global_.get() );
    for (size_t i = 0; i < args.size(); i++) {
        auto name = declaration_->params()[i].lexeme;
        auto val = args[i];
        environment->define(name, val);
    }
    interpreter.execute_block_(declaration_->body(), environment);
    return Value{};
}

std::string Function::toString() const
{
    return declaration_->name().lexeme + " :: t -> t1";
}
