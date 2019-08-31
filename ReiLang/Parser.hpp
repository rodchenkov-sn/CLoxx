#pragma once

#include <initializer_list>

#include "Lexer.hpp"
#include "Ast.hpp"

class Parser
{
public:
    Parser(std::vector<std::shared_ptr<Token>> tokens, Logger& logger);
    std::vector<Stmt::Base::Ptr> parse();
private:

    class ParserException : std::exception
    {
        [[nodiscard]] char const* what() const override { return "Parser exception."; }
    };

    Stmt::Base::Ptr declaration_();
    Stmt::Base::Ptr function_(const std::string& kind);
    Stmt::Base::Ptr var_declaration_();
    Stmt::Base::Ptr statement_();
    Stmt::Base::Ptr expression_stmt_();
    Stmt::Base::Ptr print_stmt_();
    Stmt::Base::Ptr if_statement_();
    Stmt::Base::Ptr while_loop_();
    Stmt::Base::Ptr for_loop_();
    Stmt::Base::Ptr return_();
    Stmt::Base::Ptr klass_declaration_();


    std::list<Stmt::Base::Ptr> block_();


    Expr::Base::Ptr expression_();
    Expr::Base::Ptr assignment_();
    Expr::Base::Ptr ternary_();
    Expr::Base::Ptr logic_or_();
    Expr::Base::Ptr logic_and_();
    Expr::Base::Ptr equality_();
    Expr::Base::Ptr comparison_();
    Expr::Base::Ptr addition_();
    Expr::Base::Ptr multiplication_();
    Expr::Base::Ptr unary_();
    Expr::Base::Ptr call_();
    Expr::Base::Ptr primary_();
    Expr::Base::Ptr lambda_();

    Expr::Base::Ptr finish_call_(const Expr::Base::Ptr& callee);

    bool match_(std::initializer_list<TokenType> lst);
    [[nodiscard]] bool  check_(TokenType expected) const;
    [[nodiscard]] bool  is_end_()                  const;
    [[nodiscard]] Token peek_()                    const;
    [[nodiscard]] Token previous_()                const;

    Token advance_();
    Token consume_(TokenType type, const std::string& msg);
    void advance_v_();
    void consume_v_(TokenType type, const std::string& msg);

    [[nodiscard]] ParserException error_(const Token& token, const std::string& msg) const;
    void synchronize_();

    size_t                              current_;
    std::vector<std::shared_ptr<Token>> tokens_;
    Logger&                             logger_;
};
