#include "Interpreter.hpp"
#include <iostream>

Interpreter::Interpreter(std::vector<std::shared_ptr<Stmt::Base>> statements, Logger& logger):
    statements_(std::move(statements)),
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
        for (auto& s : statements_) {
            execute_(*s);
        }
    } catch (const RuntimeError& re) {
        logger_.log(LogLevel::Error, re.line(), re.what());
        logger_.log(LogLevel::Fatal, "Bad interpreting.");
    } catch (const std::exception& e) {
        logger_.log(LogLevel::Error, e.what());
        logger_.log(LogLevel::Fatal, "Bad interpreting.");
    }
}

void Interpreter::visitExpression(Stmt::Expression& stmt)
{
    (void)evaluate_(*stmt.expr());
    
}

void Interpreter::visitPrint(Stmt::Print& stmt)
{
    const Value val = evaluate_(*stmt.expr());
    std::cout << val.toString() << "\n";
}

void Interpreter::visitVar(Stmt::Var& stmt)
{
    Value val{};
    if (stmt.expr()) {
        val = evaluate_(*stmt.expr());
    }
    environment_.define(stmt.var().lexeme, val);
}

Value Interpreter::visitAssign(Expr::Assign& expr)
{
    const Value value = evaluate_(*expr.value());
    environment_.assign(expr.name().lexeme, value);
    return value;
}

Value Interpreter::visitGrouping(Expr::Grouping& grouping)
{
    return evaluate_(*grouping.expression());
}

Value Interpreter::visitBinary(Expr::Binary& expr)
{
    try {
        const Value l = evaluate_(*expr.left());
        switch (expr.oper().type) {
        case TokenType::Plus:
            return l + evaluate_(*expr.right());
        case TokenType::Minus:
            return l - evaluate_(*expr.right());
        case TokenType::Star:
            return l * evaluate_(*expr.right());
        case TokenType::Slash:
            return l / evaluate_(*expr.right());
        case TokenType::EqualEqual:
            return l == evaluate_(*expr.right());
        case TokenType::BangEqual:
            return l != evaluate_(*expr.right());
        case TokenType::Less:
            return l < evaluate_(*expr.right());
        case TokenType::LessEqual:
            return  l <= evaluate_(*expr.right());
        case TokenType::Greater:
            return  l > evaluate_(*expr.right());
        case TokenType::GreaterEqual:
            return l >= evaluate_(*expr.right());
        case TokenType::And:
            return l.isTrue() ? l && evaluate_(*expr.right()) : Value{ false };
        case TokenType::Or:
            return l.isTrue() ? Value{ true } : l && evaluate_(*expr.right());
        default:;
        }
    } catch (const ValueOperationException& voe) {
        throw RuntimeError{ expr.oper().line, voe.what() };
    }
    // unreachable
    return Value{};
}

Value Interpreter::visitUnary(Expr::Unary& expr)
{
    try {
        const Value operand = evaluate_(*expr.operand());
        switch (expr.oper().type) {
        case TokenType::Minus:
            return -operand;
        case TokenType::Bang:
            return !operand;
        default:;
        }
    } catch (const ValueOperationException& voe) {
        throw RuntimeError{ expr.oper().line, voe.what() };
    }
    // unreachable
    return Value{};
}

Value Interpreter::visitLiteral(Expr::Literal& expr)
{
    return expr.value();
}

Value Interpreter::visitVariable(Expr::Variable& expr)
{
    try {
        return environment_.lookup(expr.name().lexeme);
    } catch (const EnvironmentException& ee) {
        throw RuntimeError{ expr.name().line, ee.what() };
    }
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

void Interpreter::execute_(Stmt::Base& stmt)
{
    stmt.accept(*this);
}
