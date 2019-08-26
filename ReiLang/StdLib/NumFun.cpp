#include "NumFun.hpp"

unsigned NumFun::arity() const
{
    return 1;
}

Value NumFun::call(Interpreter& interpreter, std::vector<Value> args)
{
    const Value value = args.front();
    switch (value.getType()) {
    case ValueType::Nil: return Value{};
    case ValueType::Bool: return value.isTrue() ? Value{ 1.0 } : Value{ 0.0 };
    case ValueType::Number: return value;
    case ValueType::String: {
        try {
            return Value{ std::stod(value.getString()) };
        } catch (...) {
            return Value{};
        }
    }
    default: return Value{};
    }
}

std::string NumFun::toString() const
{
    return "num :: t -> number";
}
