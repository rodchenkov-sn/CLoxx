#include "Parser.hpp"

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens, Logger& logger):
    current_(0),
    tokens_(std::move(tokens)),
    logger_(logger)
{
}

std::vector<Stmt::Base::Ptr> Parser::parse()
{
    std::vector<Stmt::Base::Ptr> statements;
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
    logger_.elapse("Parsing");
    return statements;
}

Stmt::Base::Ptr Parser::declaration_()
{
    try {
        if (match_({ TokenType::Class })) {
            return klass_declaration_();
        }
        if (match_({ TokenType::Fun })) {
            return function_("function");
        }
        if (match_({ TokenType::Var })) {
            return var_declaration_();
        }
        return statement_();
    } catch (const ParserException&) {
        synchronize_();
        return nullptr;
    }
}

Stmt::Base::Ptr Parser::function_(const std::string& kind)
{
    const Token name = consume_(TokenType::Identifier, "expect " + kind + " name.");
    consume_v_(TokenType::LeftParen, "expect '(' after " + kind + " name.");
    std::vector<Token> params;
    if (!check_(TokenType::RightParen)) {
        do {
            if (params.size() > 255) {
                throw error_(peek_(), "Cannot have more than 255 parameters.");
            }
            params.push_back(consume_(TokenType::Identifier, "expect parameter name"));
        } while (match_({ TokenType::Comma }));
    }
    consume_v_(TokenType::RightParen, "expect ')' after parameters.");
    consume_v_(TokenType::LeftBrace, "expect '{' before " + kind + " body.");
    auto body = block_();
    return std::make_shared<Stmt::Function>(name, params, body);
}

Stmt::Base::Ptr Parser::var_declaration_()
{
    const Token name = consume_(TokenType::Identifier, "expect variable name.");
    Expr::Base::Ptr init = nullptr;
    if (match_({ TokenType::Equal })) {
        init = expression_();
    }
    consume_v_(TokenType::Semicolon, "expect \';\' after variable declaration.");
    return std::make_shared<Stmt::Var>(name, init);
}

Stmt::Base::Ptr Parser::statement_()
{
    if (match_({ TokenType::Break, TokenType::Continue })) {
        const Token controller = previous_();
        consume_v_(TokenType::Semicolon, "expect ';' after loop controller.");
        return std::make_shared<Stmt::LoopControl>(controller);
    }
    if (match_({ TokenType::Return })) {
        return return_();
    }
    if (match_({ TokenType::While })) {
        return while_loop_();
    }
    if (match_({ TokenType::For })) {
        return for_loop_();
    }
    if (match_({ TokenType::If })) {
        return if_statement_();
    }
    if (match_({ TokenType::Print })) {
        return print_stmt_();
    }
    if (match_({TokenType::LeftBrace})) {
        return std::make_shared<Stmt::Block>(block_());
    }
    return expression_stmt_();
}

Stmt::Base::Ptr Parser::expression_stmt_()
{
    auto expr = expression_();
    consume_v_(TokenType::Semicolon, "expect \';\' after expression.");
    return std::make_shared<Stmt::Expression>(expr);
}

Stmt::Base::Ptr Parser::print_stmt_()
{
    auto val = expression_();
    consume_v_(TokenType::Semicolon, "expect \';\' after value.");
    return std::make_shared<Stmt::Print>(val);
}

Stmt::Base::Ptr Parser::if_statement_()
{
    consume_v_(TokenType::LeftParen, "expect '(' after 'if'.");
    const auto condition = expression_();
    consume_v_(TokenType::RightParen, "expect ')' after if condition.");
    const auto thenBranch = statement_();
    Stmt::Base::Ptr elseBranch = nullptr;
    if (match_({ TokenType::Else })) {
        elseBranch = statement_();
    }
    return std::make_shared<Stmt::IfStmt>(condition, thenBranch, elseBranch);
}

Stmt::Base::Ptr Parser::while_loop_()
{
    consume_v_(TokenType::LeftParen, "expect '(' after 'while'.");
    const auto condition = expression_();
    consume_v_(TokenType::RightParen, "expect ')' after while condition.");
    const auto body = statement_();
    return std::make_shared<Stmt::While>(condition, body);
}

Stmt::Base::Ptr Parser::for_loop_()
{
    consume_v_(TokenType::LeftParen, "expect '(' after 'for'.");
    Stmt::Base::Ptr init;
    if (match_({ TokenType::Semicolon })) {
        init = nullptr;
    } else if (match_({ TokenType::Var })) {
        init = var_declaration_();
    } else {
        init = expression_stmt_();
    }
    Expr::Base::Ptr condition = nullptr;
    if (!match_({ TokenType::Semicolon })) {
        condition = expression_();
        consume_v_(TokenType::Semicolon, "expect ';' after loop condition.");
    }
    Stmt::Base::Ptr incr = nullptr;
    if (!match_({ TokenType::RightParen })) {
        incr = std::make_shared<Stmt::Expression>(expression_());
        consume_v_(TokenType::RightParen, "expect ')' after for clauses.");
    }
    std::shared_ptr<Stmt::Base> body = statement_();
    if (!condition) {
        condition = std::make_shared<Expr::Literal>(Value{ true });
    }
    return std::make_shared<Stmt::Block>(std::list<Stmt::Base::Ptr>{
        std::make_shared<Stmt::ForLoop>(init, condition, incr, body)
    });
}

Stmt::Base::Ptr Parser::return_()
{
    const Token keyword = previous_();
    Expr::Base::Ptr value = nullptr;
    if (!check_(TokenType::Semicolon)) {
        value = expression_();
    }
    consume_v_(TokenType::Semicolon, "expect ';' after return value.");
    return std::make_shared<Stmt::Return>(keyword, value);
}

Stmt::Base::Ptr Parser::klass_declaration_()
{
    auto name = consume_(TokenType::Identifier, "expect class name.");
    consume_v_(TokenType::LeftBrace, "expect '{' before class body.");
    std::vector<Stmt::Base::Ptr> methods;
    while (!check_(TokenType::RightBrace) && !is_end_()) {
        methods.push_back(function_("method"));
    }
    consume_v_(TokenType::RightBrace, "expect '}' after class body.");
    return std::make_shared<Stmt::Klass>(name, methods);
}

std::list<Stmt::Base::Ptr> Parser::block_()
{
    std::list<Stmt::Base::Ptr> statements;
    while (!check_(TokenType::RightBrace) && !is_end_()) {
        statements.push_back(declaration_());
    }
    consume_v_(TokenType::RightBrace, "expect '}' after block.");
    return statements;
}

Expr::Base::Ptr Parser::expression_()
{
    return assignment_();
}

Expr::Base::Ptr Parser::assignment_()
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

Expr::Base::Ptr Parser::ternary_()
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

Expr::Base::Ptr Parser::logic_or_()
{
    auto expr = logic_and_();
    while (match_({ TokenType::Or })) {
        Token oper = previous_();
        auto right = logic_and_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

Expr::Base::Ptr Parser::logic_and_()
{
    auto expr = equality_();
    while (match_({ TokenType::And })) {
        Token oper = previous_();
        auto right = equality_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

Expr::Base::Ptr Parser::equality_()
{
    auto expr = comparison_();
    while (match_({TokenType::EqualEqual, TokenType::BangEqual})) {
        Token oper = previous_();
        auto right = comparison_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

Expr::Base::Ptr Parser::comparison_()
{
    auto expr = addition_();
    while (match_({ TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual })) {
        Token oper = previous_();
        auto right = addition_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

Expr::Base::Ptr Parser::addition_()
{
    auto expr = multiplication_();
    while (match_({ TokenType::Plus, TokenType::Minus })) {
        Token oper = previous_();
        auto right = multiplication_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

Expr::Base::Ptr Parser::multiplication_()
{
    auto expr = unary_();
    while (match_({ TokenType::Star, TokenType::Slash })) {
        Token oper = previous_();
        auto right = unary_();
        expr = std::make_shared<Expr::Binary>(expr, oper, right);
    }
    return expr;
}

Expr::Base::Ptr Parser::unary_()
{
    if (match_({ TokenType::Minus, TokenType::Bang })) {
        Token oper = previous_();
        auto operand = unary_();
        return std::make_shared<Expr::Unary>(oper, operand);
    }
    return call_();
}

Expr::Base::Ptr Parser::call_()
{
    auto expr = primary_();
    while (true) {
        if (match_({ TokenType::LeftParen })) {
            expr = finish_call_(expr);
        } else {
            break;
        }
    }
    return expr;
}

Expr::Base::Ptr Parser::primary_()
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
    if (match_({ TokenType::Fun })) {
        return lambda_();
    }
    throw error_(peek_(), "expect expression.");
}

// now, it's just a function_() copy. I'll do smth with it later
// ToDo: dry lambda method
Expr::Base::Ptr Parser::lambda_()
{
    consume_v_(TokenType::LeftParen, "expect '(' before lambda params.");
    std::vector<Token> params;
    if (!check_(TokenType::RightParen)) {
        do {
            if (params.size() > 255) {
                throw error_(peek_(), "Cannot have more than 255 parameters.");
            }
            params.push_back(consume_(TokenType::Identifier, "expect parameter name"));
        } while (match_({ TokenType::Comma }));
    }
    consume_v_(TokenType::RightParen, "expect ')' after parameters.");
    consume_v_(TokenType::LeftBrace, "expect '{' before lambda body.");
    auto body = block_();
    return std::make_shared<Expr::Lambda>(params, body);
}

Expr::Base::Ptr Parser::finish_call_(const Expr::Base::Ptr& callee)
{
    std::vector<Expr::Base::Ptr> args;
    if (!check_(TokenType::RightParen)) {
        do {
            args.push_back(expression_());
        } while (match_({ TokenType::Comma }));
    }
    if (args.size() > 255) {
        throw error_(peek_(), "cannot have more than 255 arguments.");
    }
    Token paren = consume_(TokenType::RightParen, "expect ')' after arguments.");
    return std::make_shared<Expr::Call>(callee, paren, args);
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
