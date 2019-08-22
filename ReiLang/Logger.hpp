#pragma once
#include <ostream>

enum class LogLevel
{
    Debug   = 0,
    Info    = 1,
    Warning = 2,
    Error   = 3,
    Fatal   = 4
};

const char* to_string(LogLevel e);

class Logger
{
public:
    explicit Logger(std::ostream& stream);
    void log(LogLevel level, unsigned int line, const std::string& msg);
    void log(LogLevel level, const std::string& msg);
    [[nodiscard]] unsigned int count(LogLevel level) const;
private:
    std::ostream& stream_;
    unsigned int log_count_[5];
};
