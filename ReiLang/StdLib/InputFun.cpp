#include "InputFun.hpp"
#include <iostream>

unsigned InputFun::arity() const
{
    return 0;
}

Value InputFun::call(Interpreter& interpreter, std::vector<Value> args)
{
    std::string input;
    std::cin >> input;
    return Value{ input };
}

std::string InputFun::toString() const
{
    return "input :: void -> string";
}
