#include "RandFun.hpp"
#include <random>
#include <chrono>

unsigned RandFun::arity() const
{
    return 2;
}

Value RandFun::call(Interpreter& interpreter, std::vector<Value> args)
{
    const Value low = args[0], high = args[1];
    if (low.getType() != ValueType::Number || high.getType() != ValueType::Number) {
        return Value{};
    }
    std::default_random_engine generator{ static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()) };
    std::uniform_int_distribution<int> distribution(std::floor(low.getNumber()), std::ceil(high.getNumber()));
    const int rand = distribution(generator);
    return Value{ static_cast<double>(rand) };
}

std::string RandFun::toString() const
{
    return "rand :: (num, num) -> num";
}
