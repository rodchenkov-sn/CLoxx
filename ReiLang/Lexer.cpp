#include "Lexer.hpp"

Lexer::Lexer(std::string script, Logger& logger) :
    script_(std::move(script)),
    start_(0),
    current_(0),
    line_(1),
    keywords_({
        { "and"   , TokenType::And    },
        { "class" , TokenType::Class  },
        { "else"  , TokenType::Else   },
        { "false" , TokenType::False  },
        { "for"   , TokenType::For    },
        { "fun"   , TokenType::Fun    },
        { "if"    , TokenType::If     },
        { "nil"   , TokenType::Nil    },
        { "or"    , TokenType::Or     },
        { "print" , TokenType::Print  },
        { "return", TokenType::Return },
        { "super" , TokenType::Super  },
        { "this"  , TokenType::This   },
        { "true"  , TokenType::True   },
        { "var"   , TokenType::Var    },
        { "while" , TokenType::While  }
        }),
    logger_(logger)
{
}

std::vector<std::shared_ptr<Token>> Lexer::getTokens()
{
    while (!is_end_()) {
        start_ = current_;
        get_next_token_();
    }
    tokens_.push_back(std::make_shared<Token>(TokenType::Eof, "eof", 0));
    return tokens_;
}

bool Lexer::is_end_() const
{
    return current_ >= script_.length();
}

bool Lexer::match_(const char expected)
{
    if (is_end_()) {
        return false;
    }
    if (script_[current_] == expected) {
        ++current_;
        return true;
    }
    return false;
}

char Lexer::peek_() const
{
    if (is_end_()) {
        return '\0';
    }
    return script_[current_];
}

char Lexer::peek_next_() const
{
    if (current_ + 1 >= script_.length()) {
        return '\0';
    }
    return script_[current_ + 1];
}

void Lexer::get_next_token_()
{
    const char c = advance_();
    switch (c) {
    case ' ' : [[fallthrough]] ;
    case '\t': [[fallthrough]] ;
    case '\r':          break;
    case '\n': ++line_; break;
    case '(' : add_token_(TokenType::LeftParen);  break;
    case ')' : add_token_(TokenType::RightParen); break;
    case '{' : add_token_(TokenType::LeftBrace);  break;
    case '}' : add_token_(TokenType::RightBrace); break;
    case ',' : add_token_(TokenType::Comma);      break;
    case '.' : add_token_(TokenType::Dot);        break;
    case '-' : add_token_(TokenType::Minus);      break;
    case '+' : add_token_(TokenType::Plus);       break;
    case ';' : add_token_(TokenType::Semicolon);  break;
    case '*' : add_token_(TokenType::Star);       break;
    case '=' : add_token_(match_('=') ? TokenType::EqualEqual   : TokenType::Equal);   break;
    case '!' : add_token_(match_('=') ? TokenType::BangEqual    : TokenType::Bang);    break;
    case '<' : add_token_(match_('=') ? TokenType::LessEqual    : TokenType::Less);    break;
    case '>' : add_token_(match_('=') ? TokenType::GreaterEqual : TokenType::Greater); break;
    case '/' :
        if (match_('/')) {
            while (peek_() != '\n' && !is_end_()) {
                advance_();
            }
        } else if (match_('*')) {
            const unsigned int start = line_;
            bool terminated = false;
            while (!is_end_()) {
                if (peek_() == '\n') {
                    ++line_;
                }
                if (peek_() == '*' && peek_next_() == '/') {
                    advance_();
                    advance_();
                    terminated = true;
                    break;
                }
                advance_();
            }
            if (!terminated) {
                logger_.log(LogLevel::Warning, start, "Unterminated comment.");
            }
        } else {
            add_token_(TokenType::Slash);
        }
        break;
    case '\"': make_string_(); break;
    default:  
        if (isdigit(c)) {
            make_number_();
        } else if (isalpha(c) || c == '_') {
            make_identifier_();
        } else {
            logger_.log(LogLevel::Error, line_, "Unexpected lexeme \"" + std::string(1, c) + "\".");
        }
    }
}

char Lexer::advance_()
{
    ++current_;
    return script_[current_ - 1];
}

void Lexer::add_token_(TokenType type)
{
    const std::string text = script_.substr(start_, current_ - start_);
    tokens_.push_back(std::make_shared<Token>(type, text, 0));
}

void Lexer::add_token_(double val)
{
    const std::string text = script_.substr(start_, current_ - start_);
    tokens_.push_back(std::make_shared<Token>(TokenType::Number, text, val));
}

void Lexer::add_token_(std::string val)
{
    const std::string text = script_.substr(start_, current_ - start_);
    tokens_.push_back(std::make_shared<Token>(TokenType::String, text, val));
}

void Lexer::make_string_()
{
    const unsigned int start = line_;
    while (peek_() != '"' && !is_end_()) {
        if (peek_() == '\n') {
            ++line_;
        } 
        advance_();
    }
    if (is_end_()) {
        logger_.log(LogLevel::Error, start, "Unterminated string.");
    }
    advance_();
    add_token_(script_.substr(start_ + 1, current_ - start_ - 2));
}

void Lexer::make_number_()
{
    while (isdigit(peek_())) {
        advance_();
    }
    if (peek_() == '.' && isdigit(peek_next_())) {
        advance_();
        while (isdigit(peek_())) {
            advance_();
        }
    }
    add_token_(std::stod(script_.substr(start_, current_ - start_)));
}

void Lexer::make_identifier_()
{
    while (isalpha(peek_()) || peek_() == '_' || isdigit(peek_())) {
        advance_();
    }
    const std::string text = script_.substr(start_, current_ - start_);
    if (keywords_.find(text) != keywords_.end()) {
        add_token_(keywords_[text]);
    } else {
        add_token_(TokenType::Identifier);
    }
}
