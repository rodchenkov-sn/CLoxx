#include "Ast.hpp"
#include <utility>

Expr::Grouping::Grouping(std::shared_ptr<Base> expr) :
    expression(std::move(expr))
{
}

Value Expr::Grouping::accept(Visitor& visitor)
{
    return visitor.visitGrouping(*this);
}

Expr::Binary::Binary(std::shared_ptr<Expr::Base> l, Token op, std::shared_ptr<Expr::Base> r):
    left(std::move(l)),
    oper(std::move(op)),
    right(std::move(r))
{
}

Value Expr::Binary::accept(Visitor& visitor)
{
    return visitor.visitBinary(*this);
}

Expr::Unary::Unary(Token op, std::shared_ptr<Expr::Base> n):
    oper(std::move(op)),
    operand(std::move(n))
{
}

Value Expr::Unary::accept(Visitor& visitor)
{
    return visitor.visitUnary(*this);
}

Expr::Literal::Literal(Value v):
    value(std::move(v))
{
}

Value Expr::Literal::accept(Visitor& visitor)
{
    return visitor.visitLiteral(*this);
}
