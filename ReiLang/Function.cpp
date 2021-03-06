#include "Function.hpp"
#include <utility>
#include "Interpreter.hpp"

Function::Function(Stmt::Function* declaration, std::shared_ptr<Environment> closure) :
    params_(declaration->params()),
    body_(declaration->body()),
    name_(declaration->name().lexeme),
    closure_(std::move(closure)),
	declaration_(declaration)
{
}

Function::Function(Expr::Lambda* declaration, std::shared_ptr<Environment> closure):
    params_(declaration->params()),
    body_(declaration->body()),
    name_("Lambda"),
    closure_(std::move(closure)),
	declaration_(nullptr)
{
}

unsigned Function::arity() const
{
    return params_.size();
}

Value Function::call(Interpreter& interpreter, std::vector<Value> args)
{
    auto environment = std::make_shared<Environment>( closure_.get() );
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

std::shared_ptr<Function> Function::bind(const std::weak_ptr<Instance>& instance) const
{
	auto environment = std::make_shared<Environment>(*closure_);
	environment->define("this", Value{ instance });
	return std::make_shared<Function>(declaration_, environment);
}
