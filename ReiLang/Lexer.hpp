#pragma once
#include <string>
#include <list>
#include "Token.hpp"
#include <map>

class Lexer final
{
public:
    explicit Lexer(std::string script);
    std::list<Token> getTokens();
private:

    [[nodiscard]] bool is_end_() const;
    [[nodiscard]] bool match_(char expected);
    [[nodiscard]] char peek_() const;
    [[nodiscard]] char peek_next_() const;
    void get_next_token_();
    char advance_();
    void add_token_(TokenType type);
    void add_token_(double val);
    void add_token_(std::string val);
    void make_string_();
    void make_number_();
    void make_identifier_();

    std::string script_;
    size_t start_;
    size_t current_;
    unsigned int line_;
    std::list<Token> tokens_;
    std::map<std::string, TokenType> keywords_;
};
