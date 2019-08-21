#include "ErrorHandling.hpp"
#include <iostream>

void error(unsigned int line, const std::string& error)
{
    report(line, "", error);
}

void report(unsigned int line, const std::string& where, const std::string& message)
{
    std::cerr << "[ line " << line << " ] " << where << ": " << message << "\n";
}
