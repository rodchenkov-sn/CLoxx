#include "Klass.hpp"
#include "Instance.hpp"

Klass::Klass(std::string name, std::map<std::string, std::shared_ptr<Function>> methods):
    methods_(std::move(methods)),
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

std::shared_ptr<Function> Klass::findMethod(const std::string& name) const
{
	if (methods_.find(name) != methods_.end()) {
		return methods_.at(name);
	}
	return nullptr;
}
