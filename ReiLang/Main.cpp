#include <iostream>
#include <fstream>
#include <string>

#include "Parser.hpp"

void run(const std::string& script)
{
    Logger logger{ std::cout };
    Lexer lexer = Lexer{ script, logger };
    Parser parser{ lexer.getTokens(), logger };
    std::shared_ptr<Ast::Node> ast = parser.parse();
    if (logger.count(LogLevel::Error) > 0) {
        logger.log(LogLevel::Fatal, "Bad parsing.");
        logger.log(LogLevel::Info, "Interpreting terminated due to parsing errors.");
    }
    logger.showStat();
    std::cout << "\n";
}

void runFile(const char* file)
{
    std::ifstream fin{ file };
    if (fin) {
        const std::string content{ (std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()) };
        run(content);
    }
    else {
        throw std::exception("Bad file.");
    }
}

void runPrompt()
{
    std::string expression;
    while (true) {
        std::cout << "|||| ";
        std::getline(std::cin, expression);
        if (expression.empty()) {
            continue;
        }
        if (expression == "q!") {
            break;
        }
        run(expression);
    }
}

int main(int argc, char* argv[])
{
    try {
        if (argc > 2) {
            std::cout << "Usage: rei [filepath]\n";
        }
        else if (argc == 2) {
            runFile(argv[1]);
        }
        else {
            runPrompt();
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        return 1;
    } 
    return 0;
}
