#pragma once
#include <ostream>

#define SILENCE_

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

    Logger(const Logger&)              = delete;
    Logger(Logger&&)                   = delete;
    Logger& operator = (const Logger&) = delete;
    Logger& operator = (Logger&&)      = delete;
    ~Logger()                          = default;

    void log(LogLevel level, unsigned int line, const std::string& msg);
    void log(LogLevel level, const std::string& msg);
    void clearStat();
    void showStat();
    [[nodiscard]] unsigned int count(LogLevel level) const;
private:
    std::ostream& stream_;
    unsigned int log_count_[5];
};
