#pragma once

#include <initializer_list>

#include "Lexer.hpp"
#include "Ast.hpp"

class Parser
{
public:
    Parser(std::vector<std::shared_ptr<Token>> tokens, Logger& logger);
    std::shared_ptr<Expr::Base> parse();
private:

    class ParserException : std::exception
    {
        [[nodiscard]] char const* what() const override { return "Parser exception."; }
    };

    std::shared_ptr<Expr::Base> expression_();
    std::shared_ptr<Expr::Base> logic_or_();
    std::shared_ptr<Expr::Base> logic_and_();
    std::shared_ptr<Expr::Base> equality_();
    std::shared_ptr<Expr::Base> comparison_();
    std::shared_ptr<Expr::Base> addition_();
    std::shared_ptr<Expr::Base> multiplication_();
    std::shared_ptr<Expr::Base> unary_();
    std::shared_ptr<Expr::Base> primary_();

    bool match_(std::initializer_list<TokenType> lst);
    [[nodiscard]] bool  check_(TokenType expected) const;
    [[nodiscard]] bool  is_end_()                  const;
    [[nodiscard]] Token peek_()                    const;
    [[nodiscard]] Token previous_()                const;

    [[maybe_unused]] Token advance_();
    [[maybe_unused]] Token consume_(TokenType type, const std::string& msg);

    [[nodiscard]] ParserException error_(const Token& token, const std::string& msg) const;
    void synchronize_();

    size_t                              current_;
    std::vector<std::shared_ptr<Token>> tokens_;
    Logger&                             logger_;
};
