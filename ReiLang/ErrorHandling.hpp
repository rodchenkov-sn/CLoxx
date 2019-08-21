#pragma once
#include <string>

void error(unsigned int line, const std::string& error);

void report(unsigned int line, const std::string& where, const std::string& message);
