#include "Logger.hpp"
#include <iomanip>

const char* to_string(LogLevel e)
{
    switch (e) {
    case LogLevel::Debug :
        return "Debug";
    case LogLevel::Info :
        return "Info";
    case LogLevel::Warning :
        return "Warning";
    case LogLevel::Error :
        return "Error";
    case LogLevel::Fatal :
        return "Fatal";
    default :
        return "unknown";
    }
}

Logger::Logger(std::ostream& stream):
    stream_(stream),
    log_count_{0}
{
}

void Logger::log(const LogLevel level, unsigned int line, const std::string& msg)
{
    if (level == LogLevel::Debug) {
#ifdef _DEBUG
        stream_ << std::left << std::setw(7) << to_string(level) << " [ line " << std::right << std::setw(5) << line << " ] " << msg << "\n";
#endif
    } else {
        stream_ << std::left << std::setw(7) << to_string(level) << " [ line " << std::right << std::setw(5) << line << " ] " << msg << "\n";
    }
    log_count_[int(level)]++;
}

void Logger::log(LogLevel level, const std::string& msg)
{
    if (level == LogLevel::Debug) {
#ifdef _DEBUG
        stream_ << std::left << std::setw(7) << to_string(level) << " [ XXXXXXXXXX ] " << msg << "\n";
#endif
    }
    else {
        stream_ << std::left << std::setw(7) << to_string(level) << " [ XXXXXXXXXX ] " << msg << "\n";
    }
    log_count_[int(level)]++;
}

unsigned Logger::count(LogLevel level) const
{
    return log_count_[int(level)];
}
