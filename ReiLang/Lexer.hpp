#pragma once
#include <string>
#include <vector>
#include <map>
#include "Token.hpp"
#include "Logger.hpp"

class Lexer final
{
public:
    Lexer(std::string script, Logger& logger);
    std::vector<std::shared_ptr<Token>> getTokens();
private:

    [[nodiscard]] bool is_end_() const;
    [[nodiscard]] bool match_(char expected);
    [[nodiscard]] char peek_() const;
    [[nodiscard]] char peek_next_() const;
                  char advance_();

    void get_next_token_();
    
    void add_token_(TokenType type);
    void add_token_(double val);
    void add_token_(std::string val);
    void make_string_();
    void make_number_();
    void make_identifier_();

    std::string                         script_;
    size_t                              start_;
    size_t                              current_;
    unsigned int                        line_;
    std::vector<std::shared_ptr<Token>> tokens_;
    std::map<std::string, TokenType>    keywords_;
    Logger&                             logger_;
};
