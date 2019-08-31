#pragma once
#include "Interpreter.hpp"

class Resolver : public Expr::Visitor, public Stmt::Visitor
{
public:
    Resolver(Interpreter& interpreter, Logger& logger);
    Value visitGrouping(Expr::Grouping&) override;
    Value visitTernary(Expr::Ternary&)   override;
    Value visitBinary(Expr::Binary&)     override;
    Value visitUnary(Expr::Unary&)       override;
    Value visitLiteral(Expr::Literal&)   override;
    Value visitVariable(Expr::Variable&) override;
    Value visitAssign(Expr::Assign&)     override;
    Value visitCall(Expr::Call&)         override;
    Value visitLambda(Expr::Lambda*)     override;

    void visitExpression(Stmt::Expression&) override;
    void visitPrint(Stmt::Print&)           override;
    void visitVar(Stmt::Var&)               override;
    void visitBlock(Stmt::Block&)           override;
    void visitIfStmt(Stmt::IfStmt&)         override;
    void visitWhile(Stmt::While&)           override;
    void visitControl(Stmt::LoopControl&)   override;
    void visitForLoop(Stmt::ForLoop&)       override;
    void visitFunction(Stmt::Function*)     override;
    void visitReturn(Stmt::Return&)         override;
    void visitKlass(Stmt::Klass&)           override;

    void resolve(const std::vector<Stmt::Base::Ptr>& statements);
private:

    enum class FunType
    {
        None, Function, Lambda
    };

    void resolve_(const std::vector<Stmt::Base::Ptr>& statements);
    void resolve_(const std::list<Stmt::Base::Ptr>& statements);
    void resolve_(const Stmt::Base::Ptr& statement);
    void resolve_(const Expr::Base::Ptr& expression);
    void resolve_local_(Expr::Base* expr, const Token& token);
    void resolve_function_(Stmt::Function* fun, FunType type);
    void declare_(const Token& token);
    void define_(const Token& token);
    void begin_scope_();
    void end_scope_();

    Interpreter&                             interpreter_;
    Logger&                                  logger_;
    std::vector<std::map<std::string, bool>> scopes_;
    FunType                                  current_fun_;
    bool                                     is_loop_;
};

