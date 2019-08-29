#include "Resolver.hpp"

Resolver::Resolver(Interpreter& interpreter, Logger& logger):
    interpreter_(interpreter),
    logger_(logger)
{
}

Value Resolver::visitGrouping(Expr::Grouping& expr)
{
    resolve_(expr.expression());
    return {};
}

Value Resolver::visitTernary(Expr::Ternary& expr)
{
    resolve_(expr.condition());
    resolve_(expr.ifTrue());
    resolve_(expr.ifFalse());
    return {};
}

Value Resolver::visitBinary(Expr::Binary& expr)
{
    resolve_(expr.left());
    resolve_(expr.right());
    return {};
}

Value Resolver::visitUnary(Expr::Unary& expr)
{
    resolve_(expr.operand());
    return {};
}

Value Resolver::visitLiteral(Expr::Literal&)
{
    return {};
}

Value Resolver::visitVariable(Expr::Variable& expr)
{
    if (!scopes_.empty() && scopes_.back().find(expr.name().lexeme) != scopes_.back().end() && !scopes_.back().at(expr.name().lexeme)) {
        logger_.log(LogLevel::Error, expr.name().line, "Cannot read local variable in its own initializer.");
    }
    resolve_local_(&expr, expr.name());
    return {};
}

Value Resolver::visitAssign(Expr::Assign& expr)
{
    resolve_(expr.value());
    resolve_local_(&expr, expr.name());
    return {};
}

Value Resolver::visitCall(Expr::Call& expr)
{
    resolve_(expr.callee());
    for (auto& a : expr.argument()) {
        resolve_(a);
    }
    return {};
}

Value Resolver::visitLambda(Expr::Lambda* expr)
{
    begin_scope_();
    for (auto& p : expr->params()) {
        declare_(p);
        define_(p);
    }
    resolve_(expr->body());
    end_scope_();
    return {};
}

void Resolver::visitExpression(Stmt::Expression& stmt)
{
    resolve_(stmt.expr());
}

void Resolver::visitPrint(Stmt::Print& stmt)
{
    resolve_(stmt.expr());
}

void Resolver::visitVar(Stmt::Var& stmt)
{
    declare_(stmt.var());
    if (stmt.expr()) {
        resolve_(stmt.expr());
    }
    define_(stmt.var());
}

void Resolver::visitBlock(Stmt::Block& stmt)
{
    begin_scope_();
    resolve_(stmt.statements());
    end_scope_();
}

void Resolver::visitIfStmt(Stmt::IfStmt& stmt)
{
    resolve_(stmt.condition());
    resolve_(stmt.thenBranch());
    if (stmt.elseBranch()) {
        resolve_(stmt.elseBranch());
    }
}

void Resolver::visitWhile(Stmt::While& stmt)
{
    resolve_(stmt.condition());
    resolve_(stmt.body());
}

void Resolver::visitControl(Stmt::LoopControl&)
{
    // pass;
}

void Resolver::visitForLoop(Stmt::ForLoop& stmt)
{
    begin_scope_();
    resolve_(stmt.initializer());
    resolve_(stmt.condition());
    resolve_(stmt.increment());
    resolve_(stmt.body());
    end_scope_();
}

void Resolver::visitFunction(Stmt::Function* stmt)
{
    declare_(stmt->name());
    define_(stmt->name());
    resolve_function_(stmt);
}

void Resolver::visitReturn(Stmt::Return& stmt)
{
    if (stmt.value()) {
        resolve_(stmt.value());
    }
}

void Resolver::resolve(const std::vector<Stmt::Base::Ptr>& statements)
{
    if (logger_.count(LogLevel::Fatal) > 0) {
        logger_.log(LogLevel::Info, "Syntax analyzing terminated due to fatal errors.");
        return;
    }
    resolve_(statements);
    logger_.elapse("Syntax analyzing");
}

void Resolver::resolve_(const std::vector<Stmt::Base::Ptr>& statements)
{
    for (auto& s : statements) {
        resolve_(s);
    }
}

void Resolver::resolve_(const std::list<Stmt::Base::Ptr>& statements)
{
    for (auto& s : statements) {
        resolve_(s);
    }
}

void Resolver::resolve_(const Stmt::Base::Ptr& statement)
{
    statement->accept(*this);
}

void Resolver::resolve_(const Expr::Base::Ptr& expression)
{
    (void)expression->accept(*this);
}

void Resolver::resolve_local_(Expr::Base* expr, const Token& token)
{
    for (int i = scopes_.size() - 1; i >= 0; i--) {
        if (scopes_[i].find(token.lexeme) != scopes_[i].end()) {
            interpreter_.resolve(expr, scopes_.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolve_function_(Stmt::Function* fun)
{
    begin_scope_();
    for (auto& p : fun->params()) {
        declare_(p);
        define_(p);
    }
    resolve_(fun->body());
    end_scope_();
}

void Resolver::declare_(const Token& token)
{
    if (scopes_.empty()) {
        return;
    }
    scopes_.back().insert({ token.lexeme, false });
}

void Resolver::define_(const Token& token)
{
    if (scopes_.empty()) {
        return;
    }
    scopes_.back()[token.lexeme] = true;
}

void Resolver::begin_scope_()
{
    scopes_.emplace_back();
}

void Resolver::end_scope_()
{
    scopes_.pop_back();
}
