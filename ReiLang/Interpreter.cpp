#include "Interpreter.hpp"
#include <iostream>

Interpreter::Interpreter(std::shared_ptr<Expr::Base> ast, Logger& logger):
    ast_(std::move(ast)),
    logger_(logger)
{
}

void Interpreter::interpret()
{
    if (logger_.count(LogLevel::Fatal) > 0) {
        logger_.log(LogLevel::Info, "Interpreting terminated due to fatal errors.");
        return;
    }
    try {
        const Value res = evaluate_(*ast_);
        std::cout << ".... " << res.toPrinter() << "\n";
    } catch (const RuntimeError& re) {
        logger_.log(LogLevel::Error, re.line(), re.what());
        logger_.log(LogLevel::Fatal, "Bad interpreting.");
    } catch (const std::exception& e) {
        logger_.log(LogLevel::Error, e.what());
        logger_.log(LogLevel::Fatal, "Bad interpreting.");
    }
}

Value Interpreter::visitGrouping(Expr::Grouping& grouping)
{
    return evaluate_(*grouping.expression);
}

Value Interpreter::visitBinary(Expr::Binary& expr)
{
    try {
        const Value l = evaluate_(*expr.left);
        const Value r = evaluate_(*expr.right);
        switch (expr.oper.type) {
        case TokenType::Plus:
            return l + r;
        case TokenType::Minus:
            return l - r;
        case TokenType::Star:
            return l * r;
        case TokenType::Slash:
            return l / r;
        case TokenType::EqualEqual:
            return l == r;
        case TokenType::BangEqual:
            return l != r;
        case TokenType::Less:
            return l < r;
        case TokenType::LessEqual:
            return  l <= r;
        case TokenType::Greater:
            return  l > r;
        case TokenType::GreaterEqual:
            return l >= r;
        case TokenType::And:
            return l && r;
        case TokenType::Or:
            return l || r;
        default:;
        }
    } catch (const ValueOperationException& voe) {
        throw RuntimeError{ expr.oper.line, voe.what() };
    }
    // unreachable
    return Value{};
}

Value Interpreter::visitUnary(Expr::Unary& expr)
{
    try {
        const Value operand = evaluate_(*expr.operand);
        switch (expr.oper.type) {
        case TokenType::Minus:
            return -operand;
        case TokenType::Bang:
            return !operand;
        default:;
        }
    } catch (const ValueOperationException& voe) {
        throw RuntimeError{ expr.oper.line, voe.what() };
    }
    // unreachable
    return Value{};
}

Value Interpreter::visitLiteral(Expr::Literal& expr)
{
    return expr.value;
}

Interpreter::RuntimeError::RuntimeError(const unsigned line, std::string msg):
    msg_(std::move(msg)),
    line_(line)
{
}

char const* Interpreter::RuntimeError::what() const
{
    return msg_.c_str();
}

unsigned Interpreter::RuntimeError::line() const
{
    return line_;
}

Value Interpreter::evaluate_(Expr::Base& expr)
{
    return expr.accept(*this);
}
