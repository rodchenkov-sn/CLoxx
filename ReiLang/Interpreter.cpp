#include "Interpreter.hpp"
#include "StdLib/stdlib.hpp"
#include <iostream>
#include <sstream>

Interpreter::Interpreter(std::vector<Stmt::Base::Ptr> statements, Logger& logger):
    statements_(std::move(statements)),
    logger_(logger),
    global_(std::make_shared<Environment>())
{
    global_->define("input", Value{ std::make_shared<InputFun>() });
    global_->define("num"  , Value{ std::make_shared<NumFun>()   });
    global_->define("rand" , Value{ std::make_shared<RandFun>()  });
    environment_ = global_;
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
    } catch (const LoopControl& lc) {
        logger_.log(LogLevel::Error, lc.controller().line, "Loop controller outside loop.");
    } catch (const ReturnCnt& rc) {
        logger_.log(LogLevel::Error, rc.keyword().line, "Return statement outside function.");
    } catch (const RuntimeError& re) {
        logger_.log(LogLevel::Error, re.line(), re.what());
    } catch (const std::exception& e) {
        logger_.log(LogLevel::Error, e.what());
    }
    if (logger_.count(LogLevel::Error) > 0) {
        logger_.log(LogLevel::Fatal, "Bad interpreting.");
    }
    logger_.elapse("Interpreting");
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
    environment_->define(stmt.var().lexeme, val);
}

void Interpreter::visitBlock(Stmt::Block& stmt)
{
    const auto local = std::make_shared<Environment>(environment_.get());
    execute_block_(stmt.statements(), local);
}

void Interpreter::visitIfStmt(Stmt::IfStmt& stmt)
{
    if (evaluate_(*stmt.condition()).isTrue()) {
        execute_(*stmt.thenBranch());
    } else if (stmt.elseBranch()) {
        execute_(*stmt.elseBranch());
    }
}

void Interpreter::visitWhile(Stmt::While& stmt)
{
    while (evaluate_(*stmt.condition()).isTrue()) {
        try {
            execute_(*stmt.body());
        } catch (const ContinueCnt&) {
            // continue execution
        } catch (const BreakCnt&) {
            return;
        }
    }
}

void Interpreter::visitControl(Stmt::LoopControl& stmt)
{
    if (stmt.controller().type == TokenType::Break) {
        throw BreakCnt{ stmt.controller() };
    }
    throw ContinueCnt{ stmt.controller() };
}

void Interpreter::visitForLoop(Stmt::ForLoop& stmt)
{
    if (stmt.initializer()) {
        execute_(*stmt.initializer());
    }
    while (evaluate_(*stmt.condition()).isTrue()) {
        try {
            execute_(*stmt.body());
            if (stmt.increment()) {
                execute_(*stmt.increment());
            }
        } catch (const ContinueCnt&) {
            if (stmt.increment()) {
                execute_(*stmt.increment());
            }
        } catch (const BreakCnt&) {
            return;
        }
    }
}

void Interpreter::visitFunction(Stmt::Function& stmt)
{
    const std::shared_ptr<Callable> fun = std::make_shared<Function>(std::make_shared<Stmt::Function>(stmt)); // Todo: kill it with fire
    environment_->define(stmt.name().lexeme, Value{ fun });
}

void Interpreter::visitReturn(Stmt::Return& stmt)
{
    Value val{};
    if (stmt.value()) {
        val = evaluate_(*stmt.value());
    }
    throw ReturnCnt{ stmt.keyword(), val };
}

Value Interpreter::visitCall(Expr::Call& expr)
{
    const Value callee = evaluate_(*expr.callee());
    std::vector<Value> args;
    for (auto& a : expr.argument()) {
        args.push_back(evaluate_(*a));
    }
    if (callee.getType() != ValueType::Callable) {
        throw RuntimeError{ expr.paren().line, "Can only call functions and classes." };
    }
    auto fun = callee.getCallable();
    if (args.size() != fun->arity()) {
        std::ostringstream strout;
        strout << "Expected " << fun->arity() << " arguments but got " << args.size() << ".";
        throw RuntimeError{ expr.paren().line, strout.str() };
    }
    try {
        return fun->call(*this, args);
    } catch (const LoopControl& le) {
        throw RuntimeError{ le.controller().line, "Loop controller outside loop." };
    } catch (const ReturnCnt& rc) {
        return rc.value();
    }
}

Value Interpreter::visitAssign(Expr::Assign& expr)
{
    try {
        const Value value = evaluate_(*expr.value());
        environment_->assign(expr.name().lexeme, value);
        return value;
    } catch (const EnvironmentException& ee) {
        throw RuntimeError{ expr.name().line, ee.what() };
    }
}

Value Interpreter::visitGrouping(Expr::Grouping& grouping)
{
    return evaluate_(*grouping.expression());
}

Value Interpreter::visitTernary(Expr::Ternary& expr)
{
    const Value cond = evaluate_(*expr.condition());
    if (cond.isTrue()) {
        return evaluate_(*expr.ifTrue());
    }
    return evaluate_(*expr.ifFalse());
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
        return environment_->lookup(expr.name().lexeme);
    } catch (const EnvironmentException& ee) {
        throw RuntimeError{ expr.name().line, ee.what() };
    }
}

Value Interpreter::visitLambda(Expr::Lambda& expr)
{
    const std::shared_ptr<Callable> fun = std::make_shared<Function>(std::make_shared<Expr::Lambda>(expr)); // Todo: kill it with fire
    return Value{ fun };
}

Interpreter::ContinueCnt::ContinueCnt(Token controller):
    controller_(std::move(controller))
{
}

Interpreter::BreakCnt::BreakCnt(Token controller):
    controller_(std::move(controller))
{
}

Interpreter::ReturnCnt::ReturnCnt(Token keyword, Value value):
    keyword_(std::move(keyword)),
    value_(std::move(value))
{
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

void Interpreter::execute_block_(const std::list<Stmt::Base::Ptr>& statements, std::shared_ptr<Environment> local)
{
    const auto prev = environment_;
    try {
        environment_ = local;
        for (auto& s : statements) {
            execute_(*s);
        }
        environment_ = prev;
    } catch (...) {
        environment_ = prev;
        throw;
    }
}
