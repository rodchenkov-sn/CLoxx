#pragma once
#include <memory>
#include "Token.hpp"
#include "Value.hpp"

enum class AstNodeType
{
    Grouping, Binary, Ternary, Unary, Literal, Variable, Assign,
    Expression, Print, Var
};

namespace Expr {

class Visitor;

struct Base
{
public:
    virtual ~Base() = default;
    virtual Value accept(Visitor& visitor) = 0;
    [[nodiscard]] virtual AstNodeType type() const = 0;
};

struct Grouping : Base
{
public:
    explicit Grouping(std::shared_ptr<Base> expression);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Base> expression() const { return expression_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Grouping; }
private:
    std::shared_ptr<Base> expression_;
};

struct Ternary : Base
{
public:
    Ternary(std::shared_ptr<Base> condition, std::shared_ptr<Base> ifTrue, std::shared_ptr<Base> ifFalse);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Base> condition() const { return condition_; }
    [[nodiscard]] std::shared_ptr<Base> ifTrue()    const { return if_true_;   }
    [[nodiscard]] std::shared_ptr<Base> ifFalse()   const { return if_false_;  }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Ternary; }
private:
    std::shared_ptr<Base> condition_;
    std::shared_ptr<Base> if_true_;
    std::shared_ptr<Base> if_false_;
};

struct Binary : Base
{
public:
    Binary(std::shared_ptr<Expr::Base> left, Token oper, std::shared_ptr<Expr::Base> right);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Base> left()  const { return left_;  }
    [[nodiscard]] Token                 oper()  const { return oper_;  }
    [[nodiscard]] std::shared_ptr<Base> right() const { return right_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Binary; }
private:
    std::shared_ptr<Base> left_;
    Token                 oper_;
    std::shared_ptr<Base> right_;
};

struct Unary : Base
{
public:
    Unary(Token oper, std::shared_ptr<Expr::Base> operand);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Token                 oper()    const { return oper_;    }
    [[nodiscard]] std::shared_ptr<Base> operand() const { return operand_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Unary; }
private:
    Token                 oper_;
    std::shared_ptr<Base> operand_;
};

struct Literal : Base
{
public:
    explicit Literal(Value value);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Value value() const { return value_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Literal; }
private:
    Value value_;
};

struct Variable : Base
{
public:
    explicit Variable(Token name);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Token name() const { return name_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Variable; }
private:
    Token name_;
};

struct Assign : Base
{
public:
    Assign(Token name, std::shared_ptr<Base> value);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Token                 name()  const { return name_;  }
    [[nodiscard]] std::shared_ptr<Base> value() const { return value_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Assign; }
private:
    Token name_;
    std::shared_ptr<Base> value_;
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
    virtual Value visitTernary (Ternary&)  = 0;
    virtual Value visitBinary  (Binary&)   = 0;
    virtual Value visitUnary   (Unary&)    = 0;
    virtual Value visitLiteral (Literal&)  = 0;
    virtual Value visitVariable(Variable&) = 0;
    virtual Value visitAssign  (Assign&)   = 0;
};

}

namespace Stmt {

class Visitor;

struct Base
{
public:
    virtual ~Base() = default;
    virtual void accept(Visitor& visitor) = 0;
    [[nodiscard]] virtual AstNodeType type() const = 0;
};

struct Expression : Base
{
public:
    explicit Expression(std::shared_ptr<Expr::Base> expr);
    void accept(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Expr::Base> expr() const { return expr_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Expression; }
private:
    std::shared_ptr<Expr::Base> expr_;
};

struct Print : Base
{
public:
    explicit Print(std::shared_ptr<Expr::Base> expr);
    void accept(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Expr::Base> expr() const { return expr_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Print; }
private:
    std::shared_ptr<Expr::Base> expr_;
};

struct Var : Base
{
public:
    Var(Token var, std::shared_ptr<Expr::Base> expr);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Token                       var()  const { return var_;  }
    [[nodiscard]] std::shared_ptr<Expr::Base> expr() const { return expr_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Var; }
private:
    Token var_;
    std::shared_ptr<Expr::Base> expr_;
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
    virtual void visitExpression(Expression&) = 0;
    virtual void visitPrint(Print&)           = 0;
    virtual void visitVar(Var&)               = 0;
};

}
