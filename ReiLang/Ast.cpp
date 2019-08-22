#include "Ast.hpp"
#include <utility>

Ast::Grouping::Grouping(std::shared_ptr<Node> expr) :
    expression(std::move(expr))
{
}

Ast::Binary::Binary(std::shared_ptr<Ast::Node> l, Token op, std::shared_ptr<Ast::Node> r):
    left(std::move(l)),
    oper(std::move(op)),
    right(std::move(r))
{
}

Ast::Unary::Unary(Token op, std::shared_ptr<Ast::Node> n):
    oper(std::move(op)),
    operand(std::move(n))
{
}

Ast::Literal::Literal(Value v):
    value(std::move(v))
{
}
