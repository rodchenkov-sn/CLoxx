#pragma once
#include "Ast.hpp"
#include "Logger.hpp"

class Interpreter final : Expr::Visitor
{
public:
    Interpreter(std::shared_ptr<Expr::Base> ast, Logger& logger);
    void interpret();
    Value visitGrouping(Expr::Grouping&) override;
    Value visitBinary(Expr::Binary&) override;
    Value visitUnary(Expr::Unary&) override;
    Value visitLiteral(Expr::Literal&) override;
private:
    class RuntimeError : std::exception
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

    std::shared_ptr<Expr::Base> ast_;
    Logger&                     logger_;
};
