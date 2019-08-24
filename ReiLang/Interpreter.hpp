#pragma once
#include "Ast.hpp"
#include "Logger.hpp"
#include "Environment.hpp"
#include <vector>

class Interpreter final : Expr::Visitor, Stmt::Visitor
{
public:
    Interpreter(std::vector<std::shared_ptr<Stmt::Base>> statements, Logger& logger);
    void interpret();

    void visitExpression(Stmt::Expression&) override;
    void visitPrint(Stmt::Print&)           override;
    void visitVar(Stmt::Var&)               override;

    Value visitAssign(Expr::Assign&)     override;
    Value visitGrouping(Expr::Grouping&) override;
    Value visitTernary(Expr::Ternary&)   override;
    Value visitBinary(Expr::Binary&)     override;
    Value visitUnary(Expr::Unary&)       override;
    Value visitLiteral(Expr::Literal&)   override;
    Value visitVariable(Expr::Variable&) override;
private:
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

    std::vector<std::shared_ptr<Stmt::Base>> statements_;
    Logger&                                  logger_;
    Environment                              environment_;
};
