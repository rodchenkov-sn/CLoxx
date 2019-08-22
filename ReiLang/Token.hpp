#pragma once
#include <ostream>
#include <variant>

enum class TokenType
{
    Eof,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,

    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    Identifier,
    String,
    Number,

    And,
    Class,
    Else,
    False,
    Fun,
    For,
    If,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While
};

const char* to_string(TokenType e);

struct Token
{
    Token(TokenType t, std::string lex, std::variant<double, std::string> lit, unsigned int l);

    TokenType type;
    std::string lexeme;
    std::variant<double, std::string> literal;
    unsigned int line;
    friend std::ostream& operator << (std::ostream& stream, const Token& token);
};
