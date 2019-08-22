#pragma once
#include <memory>
#include "Token.hpp"
#include "Value.hpp"

namespace Ast {

struct Node {};

struct Grouping : Node
{
    explicit Grouping(std::shared_ptr<Node> expr);
    std::shared_ptr<Node> expression;
};

struct Binary : Node
{
    Binary(std::shared_ptr<Ast::Node> l, Token op, std::shared_ptr<Ast::Node> r);
    std::shared_ptr<Node> left;
    Token                 oper;
    std::shared_ptr<Node> right;
};

struct Unary : Node
{
    Unary(Token op, std::shared_ptr<Ast::Node> n);
    Token                 oper;
    std::shared_ptr<Node> operand;
};

struct Literal : Node
{
    explicit Literal(Value v);
    Value value;
};

}
