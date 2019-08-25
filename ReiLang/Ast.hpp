#pragma once
#include <memory>
#include "Token.hpp"
#include "Value.hpp"
#include <list>

enum class AstNodeType
{
    Grouping, Binary, Ternary, Unary, Literal, Variable, Assign,
    Expression, Print, Var, Block, IfStmt, While, Controller
};

namespace Expr {

class Visitor;

class Base
{
public:
    typedef std::shared_ptr<Base> Ptr;
    virtual ~Base() = default;
    virtual Value accept(Visitor& visitor) = 0;
    [[nodiscard]] virtual AstNodeType type() const = 0;
};

class Grouping : public Base
{
public:
    explicit Grouping(std::shared_ptr<Base> expression);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Base> expression() const { return expression_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Grouping; }
private:
    std::shared_ptr<Base> expression_;
};

class Ternary : public Base
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

class Binary : public Base
{
public:
    Binary(Expr::Base::Ptr left, Token oper, Expr::Base::Ptr right);
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

class Unary : public Base
{
public:
    Unary(Token oper, Expr::Base::Ptr operand);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Token                 oper()    const { return oper_;    }
    [[nodiscard]] std::shared_ptr<Base> operand() const { return operand_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Unary; }
private:
    Token                 oper_;
    std::shared_ptr<Base> operand_;
};

class Literal : public Base
{
public:
    explicit Literal(Value value);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Value value() const { return value_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Literal; }
private:
    Value value_;
};

class Variable : public Base
{
public:
    explicit Variable(Token name);
    Value accept(Visitor& visitor) override;

    [[nodiscard]] Token name() const { return name_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Variable; }
private:
    Token name_;
};

class Assign : public Base
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

class Base
{
public:
    typedef std::shared_ptr<Base> Ptr;
    virtual ~Base() = default;
    virtual void accept(Visitor& visitor) = 0;
    [[nodiscard]] virtual AstNodeType type() const = 0;
};

class Expression : public Base
{
public:
    explicit Expression(Expr::Base::Ptr expr);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Expr::Base::Ptr expr() const { return expr_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Expression; }
private:
    Expr::Base::Ptr expr_;
};

class Print : public Base
{
public:
    explicit Print(Expr::Base::Ptr expr);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Expr::Base::Ptr expr() const { return expr_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Print; }
private:
    Expr::Base::Ptr expr_;
};

class Var : public Base
{
public:
    Var(Token var, Expr::Base::Ptr expr);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Token                       var()  const { return var_;  }
    [[nodiscard]] Expr::Base::Ptr expr() const { return expr_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Var; }
private:
    Token var_;
    Expr::Base::Ptr expr_;
};

class Block : public Base
{
public:
    explicit Block(std::list<Ptr> statements);
    void accept(Visitor& visitor) override;

    [[nodiscard]] const std::list<Ptr>& statements() const { return statements_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Block; }
private:
    std::list<Ptr> statements_;
};

class IfStmt : public Base
{
public:
    IfStmt(Expr::Base::Ptr condition, Stmt::Base::Ptr thenBranch, Stmt::Base::Ptr elseBranch);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Expr::Base::Ptr condition()  const { return condition_;   }
    [[nodiscard]] Stmt::Base::Ptr thenBranch() const { return then_branch_; }
    [[nodiscard]] Stmt::Base::Ptr elseBranch() const { return else_branch_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::IfStmt; }
private:
    Expr::Base::Ptr condition_;
    Stmt::Base::Ptr then_branch_;
    Stmt::Base::Ptr else_branch_;
};

class While : public Base
{
public:
    While(Expr::Base::Ptr condition, Stmt::Base::Ptr body);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Expr::Base::Ptr condition() const { return condition_; }
    [[nodiscard]] Stmt::Base::Ptr body()      const { return body_;      }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::While; }
private:
    Expr::Base::Ptr condition_;
    Stmt::Base::Ptr body_;
};

class LoopControl : public Base
{
public:
    explicit LoopControl(Token controller);
    void accept(Visitor& visitor) override;

    [[nodiscard]] Token controller() const { return controller_; }

    [[nodiscard]] AstNodeType type() const override { return AstNodeType::Controller; }
private:
    Token controller_;
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
    virtual void visitBlock(Block&)           = 0;
    virtual void visitIfStmt(IfStmt&)         = 0;
    virtual void visitWhile(While&)           = 0;
    virtual void visitControl(LoopControl&)   = 0;
};

}
