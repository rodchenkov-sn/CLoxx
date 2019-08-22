#include "Parser.hpp"

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens, Logger& logger):
    current_(0),
    tokens_(std::move(tokens)),
    logger_(logger)
{
}

std::shared_ptr<Ast::Node> Parser::parse()
{
    if (logger_.count(LogLevel::Fatal) > 0) {
        logger_.log(LogLevel::Info, "Parsing terminated due to lexer errors.");
        return nullptr;
    }
    return expression_();
}

std::shared_ptr<Ast::Node> Parser::expression_()
{
    try {
        return equality_();
    } catch (const ParserException& pe) {
        return nullptr;
    }
}

std::shared_ptr<Ast::Node> Parser::equality_()
{
    std::shared_ptr<Ast::Node> expr = comparison_();
    while (match_({TokenType::EqualEqual, TokenType::BangEqual})) {
        Token oper = previous_();
        std::shared_ptr<Ast::Node> right = comparison_();
        expr = std::make_shared<Ast::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Ast::Node> Parser::comparison_()
{
    std::shared_ptr<Ast::Node> expr = addition_();
    while (match_({ TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual })) {
        Token oper = previous_();
        std::shared_ptr<Ast::Node> right = addition_();
        expr = std::make_shared<Ast::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Ast::Node> Parser::addition_()
{
    std::shared_ptr<Ast::Node> expr = multiplication_();
    while (match_({ TokenType::Plus, TokenType::Minus })) {
        Token oper = previous_();
        std::shared_ptr<Ast::Node> right = multiplication_();
        expr = std::make_shared<Ast::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Ast::Node> Parser::multiplication_()
{
    std::shared_ptr<Ast::Node> expr = unary_();
    while (match_({ TokenType::Star, TokenType::Slash })) {
        Token oper = previous_();
        std::shared_ptr<Ast::Node> right = unary_();
        expr = std::make_shared<Ast::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Ast::Node> Parser::unary_()
{
    if (match_({ TokenType::Minus, TokenType::Bang })) {
        Token oper = previous_();
        std::shared_ptr<Ast::Node> operand = unary_();
        return std::make_shared<Ast::Unary>(oper, operand);
    }
    return primary_();
}

std::shared_ptr<Ast::Node> Parser::primary_()
{
    if (match_({ TokenType::False })) {
        return std::make_shared<Ast::Literal>(Value{ false });
    }
    if (match_({ TokenType::True })) {
        return std::make_shared<Ast::Literal>(Value{ true });
    }
    if (match_({ TokenType::Nil })) {
        return std::make_shared<Ast::Literal>(Value{});
    }
    if (match_({ TokenType::Number })) {
        return std::make_shared<Ast::Literal>(Value{ std::get<double>(previous_().literal) });
    }
    if (match_({ TokenType::String })) {
        return std::make_shared<Ast::Literal>(Value{ std::get<std::string>(previous_().literal) });
    }
    if (match_({ TokenType::LeftParen })) {
        std::shared_ptr<Ast::Node> expr = expression_();
        (void)consume_(TokenType::RightParen, "expect ')' after expression.");
        return std::make_shared<Ast::Grouping>(expr);
    }
    throw error_(peek_(), "expect expression.");
}

bool Parser::match_(std::initializer_list<TokenType> lst)
{
    for (auto& t : lst) {
        if (check_(t)) {
            (void)advance_();
            return true;
        }
    }
    return false;
}

bool Parser::check_(const TokenType expected) const
{
    if (is_end_()) {
        return false;
    }
    return peek_().type == expected;
}

bool Parser::is_end_() const
{
    return peek_().type == TokenType::Eof;
}

Token Parser::peek_() const
{
    return *tokens_[current_];
}

Token Parser::previous_() const
{
    return *tokens_[current_ - 1];
}

Token Parser::advance_()
{
    if (!is_end_()) {
        ++current_;
    }
    return previous_();
}

Token Parser::consume_(TokenType type, const std::string& msg)
{
    if (check_(type)) {
        return advance_();
    }
    throw error_(peek_(), msg);
}

Parser::ParserException Parser::error_(const Token& token, const std::string& msg) const
{
    logger_.log(LogLevel::Error, token.line, "At token \'" + token.lexeme + "\' " + msg);
    return ParserException{};
}

void Parser::synchronize_()
{
    advance_();
    while (!is_end_()) {
        
        if (previous_().type == TokenType::Semicolon) {
            return;
        }

        switch (peek_().type) {
        case TokenType::Class:  [[fallthrough]] ;
        case TokenType::Fun:    [[fallthrough]] ;
        case TokenType::Var:    [[fallthrough]] ;
        case TokenType::For:    [[fallthrough]] ;
        case TokenType::If:     [[fallthrough]] ;
        case TokenType::While:  [[fallthrough]] ;
        case TokenType::Print:  [[fallthrough]] ;
        case TokenType::Return: return;
        default:;
        }

    }
}
