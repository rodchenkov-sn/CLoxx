#pragma once
#include <memory>
#include "Token.hpp"
#include "Value.hpp"

namespace Expr {

class Visitor;

struct Base
{
    virtual ~Base() = default;
    virtual Value accept(Visitor& visitor) = 0;
};

struct Grouping : Base
{
    explicit Grouping(std::shared_ptr<Base> expr);
    Value accept(Visitor& visitor) override;
    std::shared_ptr<Base> expression;
};

struct Binary : Base
{
    Binary(std::shared_ptr<Expr::Base> l, Token op, std::shared_ptr<Expr::Base> r);
    Value accept(Visitor& visitor) override;
    std::shared_ptr<Base> left;
    Token                 oper;
    std::shared_ptr<Base> right;
};

struct Unary : Base
{
    Unary(Token op, std::shared_ptr<Expr::Base> n);
    Value accept(Visitor& visitor) override;
    Token                 oper;
    std::shared_ptr<Base> operand;
};

struct Literal : Base
{
    explicit Literal(Value v);
    Value accept(Visitor& visitor) override;
    Value value;
};

class Visitor
{
public:
    Visitor()                            = default;
    Visitor(const Visitor&)              = delete;
    Visitor(Visitor&&)                   = delete;
    Visitor& operator = (const Visitor&) = delete;
    Visitor& operator = (Visitor&&)      = delete;
    virtual ~Visitor()                   = default;
    virtual Value visitGrouping(Grouping&) = 0;
    virtual Value visitBinary  (Binary&)   = 0;
    virtual Value visitUnary   (Unary&)    = 0;
    virtual Value visitLiteral (Literal&)  = 0;
};

}
