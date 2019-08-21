#include "Token.hpp"

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    stream << "(" << token.lexeme << " :: " << to_string(token.type);
    if (token.type == TokenType::Number) {
        stream << " = " << std::get<double>(token.literal);
    }
    if (token.type == TokenType::String) {
        stream << " = " << std::get<std::string>(token.literal);
    }
    stream << ")";
    return stream;
}

const char* to_string(TokenType e)
{
    switch (e) {
    case TokenType::Eof :
        return "Eof";
    case TokenType::LeftParen :
        return "LeftParen";
    case TokenType::RightParen :
        return "RightParen";
    case TokenType::LeftBrace :
        return "LeftBrace";
    case TokenType::RightBrace :
        return "RightBrace";
    case TokenType::Comma :
        return "Comma";
    case TokenType::Dot :
        return "Dot";
    case TokenType::Minus :
        return "Minus";
    case TokenType::Plus :
        return "Plus";
    case TokenType::Semicolon :
        return "Semicolon";
    case TokenType::Slash :
        return "Slash";
    case TokenType::Star :
        return "Star";
    case TokenType::Bang :
        return "Bang";
    case TokenType::BangEqual :
        return "BangEqual";
    case TokenType::Equal :
        return "Equal";
    case TokenType::EqualEqual :
        return "EqualEqual";
    case TokenType::Greater :
        return "Greater";
    case TokenType::GreaterEqual :
        return "GreaterEqual";
    case TokenType::Less :
        return "Less";
    case TokenType::LessEqual :
        return "LessEqual";
    case TokenType::Identifier :
        return "Identifier";
    case TokenType::String :
        return "String";
    case TokenType::Number :
        return "Number";
    case TokenType::And :
        return "And";
    case TokenType::Class :
        return "Class";
    case TokenType::Else :
        return "Else";
    case TokenType::False :
        return "False";
    case TokenType::Fun :
        return "Fun";
    case TokenType::For :
        return "For";
    case TokenType::If :
        return "If";
    case TokenType::Nil :
        return "Nil";
    case TokenType::Or :
        return "Or";
    case TokenType::Print :
        return "Print";
    case TokenType::Return :
        return "Return";
    case TokenType::Super :
        return "Super";
    case TokenType::This :
        return "This";
    case TokenType::True :
        return "True";
    case TokenType::Var :
        return "Var";
    case TokenType::While :
        return "While";
    default :
        return "unknown";
    }
}
