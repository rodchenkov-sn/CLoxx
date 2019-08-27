#include "Ast.hpp"
#include <utility>

Expr::Call::Call(Expr::Base::Ptr callee, Token paren, std::vector<Expr::Base::Ptr> arguments):
    callee_(std::move(callee)),
    paren_(std::move(paren)),
    arguments_(std::move(arguments))
{
}

Value Expr::Call::accept(Visitor& visitor)
{
    return visitor.visitCall(*this);
}

Expr::Grouping::Grouping(std::shared_ptr<Base> expression) :
    expression_(std::move(expression))
{
}

Value Expr::Grouping::accept(Visitor& visitor)
{
    return visitor.visitGrouping(*this);
}

Expr::Ternary::Ternary(std::shared_ptr<Base> condition, std::shared_ptr<Base> ifTrue, std::shared_ptr<Base> ifFalse):
    condition_(std::move(condition)),
    if_true_(std::move(ifTrue)),
    if_false_(std::move(ifFalse))
{
}

Value Expr::Ternary::accept(Visitor& visitor)
{
    return visitor.visitTernary(*this);
}

Expr::Binary::Binary(Expr::Base::Ptr left, Token oper, Expr::Base::Ptr right):
    left_(std::move(left)),
    oper_(std::move(oper)),
    right_(std::move(right))
{
}

Value Expr::Binary::accept(Visitor& visitor)
{
    return visitor.visitBinary(*this);
}

Expr::Unary::Unary(Token oper, Expr::Base::Ptr operand):
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

Expr::Lambda::Lambda(std::vector<Token> params, std::list<Stmt::Base::Ptr> body):
    params_(std::move(params)),
    body_(std::move(body))
{
}

Value Expr::Lambda::accept(Visitor& visitor)
{
    return visitor.visitLambda(*this);
}

Stmt::Expression::Expression(Expr::Base::Ptr expr):
    expr_(std::move(expr))
{
}

void Stmt::Expression::accept(Visitor& visitor)
{
    visitor.visitExpression(*this);
}

Stmt::Print::Print(Expr::Base::Ptr expr):
    expr_(std::move(expr))
{
}

void Stmt::Print::accept(Visitor& visitor)
{
    visitor.visitPrint(*this);
}

Stmt::Var::Var(Token var, Expr::Base::Ptr expr):
    var_(std::move(var)),
    expr_(std::move(expr))
{
}

void Stmt::Var::accept(Visitor& visitor)
{
    visitor.visitVar(*this);
}

Stmt::Block::Block(std::list<Ptr> statements):
    statements_(std::move(statements))
{
}

void Stmt::Block::accept(Visitor& visitor)
{
    visitor.visitBlock(*this);
}

Stmt::IfStmt::IfStmt(Expr::Base::Ptr condition, Stmt::Base::Ptr thenBranch, Stmt::Base::Ptr elseBranch):
    condition_(std::move(condition)),
    then_branch_(std::move(thenBranch)),
    else_branch_(std::move(elseBranch))
{
}

void Stmt::IfStmt::accept(Visitor& visitor)
{
    visitor.visitIfStmt(*this);
}

Stmt::While::While(Expr::Base::Ptr condition, Stmt::Base::Ptr body):
    condition_(std::move(condition)),
    body_(std::move(body))
{
}

void Stmt::While::accept(Visitor& visitor)
{
    visitor.visitWhile(*this);
}

Stmt::LoopControl::LoopControl(Token controller):
    controller_(std::move(controller))
{
}

void Stmt::LoopControl::accept(Visitor& visitor)
{
    visitor.visitControl(*this);
}

Stmt::ForLoop::ForLoop(Stmt::Base::Ptr initializer, Expr::Base::Ptr condition, Stmt::Base::Ptr increment,
    Stmt::Base::Ptr body):
    initializer_(std::move(initializer)),
    condition_(std::move(condition)),
    increment_(std::move(increment)),
    body_(std::move(body))
{
}

void Stmt::ForLoop::accept(Visitor& visitor)
{
    visitor.visitForLoop(*this);
}

Stmt::Function::Function(Token name, std::vector<Token> params, std::list<Stmt::Base::Ptr> body):
    name_(std::move(name)),
    params_(std::move(params)),
    body_(std::move(body))
{
}

void Stmt::Function::accept(Visitor& visitor)
{
    visitor.visitFunction(*this);
}

Stmt::Return::Return(Token keyword, Expr::Base::Ptr value):
    keyword_(std::move(keyword)),
    value_(std::move(value))
{
}

void Stmt::Return::accept(Visitor& visitor)
{
    visitor.visitReturn(*this);
}
