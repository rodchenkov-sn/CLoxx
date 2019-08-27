#include "Logger.hpp"
#include <iomanip>
#include <sstream>

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

Logger::Logger(std::ostream& stream) :
    stream_(stream),
    log_count_{ 0 },
    start_(std::chrono::high_resolution_clock::now())
{
}

void Logger::log(const LogLevel level, unsigned int line, const std::string& msg)
{
#ifndef SILENCE
    if (level == LogLevel::Debug) {
    #ifdef _DEBUG
        stream_ << std::left << std::setw(7) << to_string(level) << " [ line " << std::right << std::setw(5) << line << " ] " << msg << "\n";
    #endif
    } else {
        stream_ << std::left << std::setw(7) << to_string(level) << " [ line " << std::right << std::setw(5) << line << " ] " << msg << "\n";
    }
#endif
    log_count_[int(level)]++;
}

void Logger::log(LogLevel level, const std::string& msg)
{
#ifndef SILENCE
    if (level == LogLevel::Debug) {
    #ifdef _DEBUG
        stream_ << std::left << std::setw(7) << to_string(level) << " [            ] " << msg << "\n";
    #endif
    }
    else {
        stream_ << std::left << std::setw(7) << to_string(level) << " [            ] " << msg << "\n";
    }
#endif
    log_count_[int(level)]++;
}

void Logger::elapse(const std::string& event)
{
    const auto end = std::chrono::high_resolution_clock::now();
    const auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(start_).time_since_epoch().count();
    const auto stop  = std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch().count();
    const auto duration = stop - start;
    start_ = end;
    std::ostringstream strout;
    strout << duration;
    std::string str = strout.str();
    log(LogLevel::Info, event + " duration: " + str + " ms.");
}

void Logger::clearStat()
{
    for (auto& i : log_count_) {
        i = 0;
    }
}

void Logger::showStat()
{
    stream_ << "\n===== Total: warnings: " << log_count_[int(LogLevel::Warning)] << ", errors: " << log_count_[int(LogLevel::Error)] << " =====\n";
}

unsigned Logger::count(LogLevel level) const
{
    return log_count_[int(level)];
}
