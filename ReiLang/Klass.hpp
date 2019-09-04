#pragma once
#include <string>
#include "Callable.hpp"
#include <map>

class Function;

class Klass : public Callable
{
public:
    Klass(std::string name, std::map<std::string, std::shared_ptr<Function>> methods);
    [[nodiscard]] std::string toString() const override;
    Value call(Interpreter& interpreter, std::vector<Value> args) override;
    [[nodiscard]] unsigned arity() const override;
	[[nodiscard]] std::shared_ptr<Function> findMethod(const std::string& name) const;
private:
	std::map<std::string, std::shared_ptr<Function>> methods_;
	std::string name_;
};

