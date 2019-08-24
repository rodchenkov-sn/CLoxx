#include "Parser.hpp"

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens, Logger& logger):
    current_(0),
    tokens_(std::move(tokens)),
    logger_(logger)
{
}

std::vector<std::shared_ptr<Stmt::Base>> Parser::parse()
{
    std::vector<std::shared_ptr<Stmt::Base>> statements;
    if (logger_.count(LogLevel::Fatal) > 0) {
        logger_.log(LogLevel::Info, "Parsing terminated due to fatal errors.");
        return statements;
    }
    while (!is_end_()) {
        statements.push_back(declaration_());
    }
    if (logger_.count(LogLevel::Error) > 0) {
        logger_.log(LogLevel::Fatal, "Bad parsing.");
    }
    return statements;
}

std::shared_ptr<Stmt::Base> Parser::declaration_()
{
    try {
        if (match_({ TokenType::Var })) {
            return var_declaration_();
        }
        return statement_();
    } catch (const ParserException&) {
        synchronize_();
        return nullptr;
    }
}

std::shared_ptr<Stmt::Base> Parser::var_declaration_()
{
    const Token name = consume_(TokenType::Identifier, "expect variable name.");
    std::shared_ptr<Expr::Base> init = nullptr;
    if (match_({ TokenType::Equal })) {
        init = expression_();
    }
    consume_v_(TokenType::Semicolon, "expect \';\' after variable declaration.");
    return std::make_shared<Stmt::Var>(name, init);
}

std::shared_ptr<Stmt::Base> Parser::statement_()
{
    if (match_({ TokenType::Print })) {
        return print_stmt_();
    }
    return expression_stmt_();
}

std::shared_ptr<Stmt::Base> Parser::expression_stmt_()
{
    auto expr = expression_();
    consume_v_(TokenType::Semicolon, "expect \';\' after expression.");
    return std::make_shared<Stmt::Expression>(expr);
}

std::shared_ptr<Stmt::Base> Parser::print_stmt_()
{
    auto val = expression_();
    consume_v_(TokenType::Semicolon, "expect \';\' after value.");
    return std::make_shared<Stmt::Print>(val);
}

std::shared_ptr<Expr::Base> Parser::expression_()
{
    return assignment_();
}

std::shared_ptr<Expr::Base> Parser::assignment_()
{
    const auto expr = ternary_();
    if (match_({TokenType::Equal})) {
        const Token equals = previous_();
        auto val = assignment_();
        if (expr->type() == AstNodeType::Variable) {
            Token name = dynamic_cast<Expr::Variable*>(expr.get())->name();
            return std::make_shared<Expr::Assign>(name, val);
        }
        throw error_(equals, "Invalid assignment target.");
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::ternary_()
{
    const auto cond = logic_or_();
    if (match_({ TokenType::QuestionMark })) {
        auto ifTrue = ternary_();
        consume_v_(TokenType::Colon, "expect \':\' after ternary option.");
        auto ifFalse = ternary_();
        return std::make_shared<Expr::Ternary>(cond, ifTrue, ifFalse);
    }
    return cond;
}

std::shared_ptr<Expr::Base> Parser::logic_or_()
{
    auto expr = logic_and_();
    while (match_({ TokenType::Or })) {
        Token oper = previous_();
        auto right = logic_and_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::logic_and_()
{
    auto expr = equality_();
    while (match_({ TokenType::And })) {
        Token oper = previous_();
        auto right = equality_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::equality_()
{
    auto expr = comparison_();
    while (match_({TokenType::EqualEqual, TokenType::BangEqual})) {
        Token oper = previous_();
        auto right = comparison_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::comparison_()
{
    auto expr = addition_();
    while (match_({ TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual })) {
        Token oper = previous_();
        auto right = addition_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::addition_()
{
    auto expr = multiplication_();
    while (match_({ TokenType::Plus, TokenType::Minus })) {
        Token oper = previous_();
        auto right = multiplication_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::multiplication_()
{
    auto expr = unary_();
    while (match_({ TokenType::Star, TokenType::Slash })) {
        Token oper = previous_();
        auto right = unary_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

std::shared_ptr<Expr::Base> Parser::unary_()
{
    if (match_({ TokenType::Minus, TokenType::Bang })) {
        Token oper = previous_();
        auto operand = unary_();
        return std::make_shared<Expr::Unary>(oper, operand);
    }
    return primary_();
}

std::shared_ptr<Expr::Base> Parser::primary_()
{
    if (match_({ TokenType::False })) {
        return std::make_shared<Expr::Literal>(Value{ false });
    }
    if (match_({ TokenType::True })) {
        return std::make_shared<Expr::Literal>(Value{ true });
    }
    if (match_({ TokenType::Nil })) {
        return std::make_shared<Expr::Literal>(Value{});
    }
    if (match_({ TokenType::Number })) {
        return std::make_shared<Expr::Literal>(Value{ std::get<double>(previous_().literal) });
    }
    if (match_({ TokenType::String })) {
        return std::make_shared<Expr::Literal>(Value{ std::get<std::string>(previous_().literal) });
    }
    if (match_({ TokenType::LeftParen })) {
        auto expr = expression_();
        consume_v_(TokenType::RightParen, "expect ')' after expression.");
        return std::make_shared<Expr::Grouping>(expr);
    }
    if (match_({ TokenType::Identifier })) {
        return std::make_shared<Expr::Variable>(previous_());
    }
    throw error_(peek_(), "expect expression.");
}

bool Parser::match_(std::initializer_list<TokenType> lst)
{
    for (auto& t : lst) {
        if (check_(t)) {
            advance_v_();
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

Token Parser::consume_(const TokenType type, const std::string& msg)
{
    if (check_(type)) {
        return advance_();
    }
    throw error_(peek_(), msg);
}

void Parser::advance_v_()
{
    if (!is_end_()) {
        current_++;
    }
}

void Parser::consume_v_(const TokenType type, const std::string& msg)
{
    if (check_(type)) {
        advance_v_();
    } else {
        throw error_(peek_(), msg);
    }
}

Parser::ParserException Parser::error_(const Token& token, const std::string& msg) const
{
    logger_.log(LogLevel::Error, token.line, "At token \'" + token.lexeme + "\' " + msg);
    return ParserException{};
}

void Parser::synchronize_()
{
    advance_v_();
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

        advance_v_();

    }
}
