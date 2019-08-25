#pragma once
#include "Ast.hpp"
#include "Logger.hpp"
#include "Environment.hpp"
#include <vector>

class Interpreter final : Expr::Visitor, Stmt::Visitor
{
public:
    Interpreter(std::vector<Stmt::Base::Ptr> statements, Logger& logger);
    void interpret();

    void visitExpression(Stmt::Expression&) override;
    void visitPrint(Stmt::Print&)           override;
    void visitVar(Stmt::Var&)               override;
    void visitBlock(Stmt::Block&)           override;
    void visitIfStmt(Stmt::IfStmt&)         override;
    void visitWhile(Stmt::While&)           override;
    void visitControl(Stmt::LoopControl&)   override;
    void visitForLoop(Stmt::ForLoop&)       override;

    Value visitAssign(Expr::Assign&)     override;
    Value visitGrouping(Expr::Grouping&) override;
    Value visitTernary(Expr::Ternary&)   override;
    Value visitBinary(Expr::Binary&)     override;
    Value visitUnary(Expr::Unary&)       override;
    Value visitLiteral(Expr::Literal&)   override;
    Value visitVariable(Expr::Variable&) override;
private:

    class LoopControl
    {
    public:
        virtual ~LoopControl() = default;
        [[nodiscard]] virtual Token controller() const = 0;
    };

    class ContinueCnt : public LoopControl
    {
    public:
        explicit ContinueCnt(Token controller);
        [[nodiscard]] Token controller() const override { return controller_; }
    private:
        Token controller_;
    };

    class BreakCnt : public LoopControl
    {
    public:
        explicit BreakCnt(Token controller);
        [[nodiscard]] Token controller() const override { return controller_; }
    private:
        Token controller_;
    };

    class RuntimeError final : std::exception
    {
    public:
        RuntimeError(unsigned int line, std::string msg);
        [[nodiscard]] char const*  what() const override;
        [[nodiscard]] unsigned int line() const;
    private:
        std::string msg_;
        unsigned int line_;
    };

    Value evaluate_(Expr::Base& expr);
    void execute_(Stmt::Base& stmt);
    void execute_block_(const std::list<Stmt::Base::Ptr>& statements, std::shared_ptr<Environment> local);

    std::vector<Stmt::Base::Ptr> statements_;
    Logger&                      logger_;
    std::shared_ptr<Environment> environment_;
};
