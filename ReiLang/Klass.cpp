#include "Klass.hpp"
#include "Instance.hpp"

Klass::Klass(std::string name):
    name_(std::move(name))
{
}

std::string Klass::toString() const
{
    return name_;
}

Value Klass::call(Interpreter& interpreter, std::vector<Value> args)
{
    return Value{ std::make_shared<Instance>(this) };
}

unsigned Klass::arity() const
{
    return 0;
}
