#include "Ast.hpp"
#include <utility>

Expr::Grouping::Grouping(std::shared_ptr<Base> expression) :
    expression_(std::move(expression))
{
}

Value Expr::Grouping::accept(Visitor& visitor)
{
    return visitor.visitGrouping(*this);
}

Expr::Binary::Binary(std::shared_ptr<Expr::Base> left, Token oper, std::shared_ptr<Expr::Base> right):
    left_(std::move(left)),
    oper_(std::move(oper)),
    right_(std::move(right))
{
}

Value Expr::Binary::accept(Visitor& visitor)
{
    return visitor.visitBinary(*this);
}

Expr::Unary::Unary(Token oper, std::shared_ptr<Expr::Base> operand):
    oper_(std::move(oper)),
    operand_(std::move(operand))
{
}

Value Expr::Unary::accept(Visitor& visitor)
{
    return visitor.visitUnary(*this);
}

Expr::Literal::Literal(Value value):
    value_(std::move(value))
{
}

Value Expr::Literal::accept(Visitor& visitor)
{
    return visitor.visitLiteral(*this);
}

Expr::Variable::Variable(Token name):
    name_(std::move(name))
{
}

Value Expr::Variable::accept(Visitor& visitor)
{
    return visitor.visitVariable(*this);
}

Expr::Assign::Assign(Token name, std::shared_ptr<Base> value):
    name_(std::move(name)),
    value_(std::move(value))
{
}

Value Expr::Assign::accept(Visitor& visitor)
{
    return visitor.visitAssign(*this);
}

Stmt::Expression::Expression(std::shared_ptr<Expr::Base> expr):
    expr_(std::move(expr))
{
}

void Stmt::Expression::accept(Visitor& visitor)
{
    visitor.visitExpression(*this);
}

Stmt::Print::Print(std::shared_ptr<Expr::Base> expr):
    expr_(std::move(expr))
{
}

void Stmt::Print::accept(Visitor& visitor)
{
    visitor.visitPrint(*this);
}

Stmt::Var::Var(Token var, std::shared_ptr<Expr::Base> expr):
    var_(std::move(var)),
    expr_(std::move(expr))
{
}

void Stmt::Var::accept(Visitor& visitor)
{
    visitor.visitVar(*this);
}
