#pragma once

/**
 * @file logger.hpp
 * @brief Thread-safe logging system for FileForge
 */

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

#include "types.hpp"

namespace fileforge {

/**
 * @brief Log level enumeration
 */
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

/**
 * @brief Convert LogLevel to string
 */
constexpr const char* logLevelToString(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

/**
 * @brief Thread-safe logger with JSON Lines output
 */
class Logger {
public:
    /**
     * @brief Get the singleton instance
     */
    static Logger& instance();

    // Delete copy/move constructors
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * @brief Initialize the logger with a file path
     * @param logPath Path to the log file
     * @param consoleOutput Also output to console
     * @return true if initialization successful
     */
    bool initialize(const fs::path& logPath, bool consoleOutput = true);

    /**
     * @brief Close the logger and flush all data
     */
    void close();

    /**
     * @brief Set the minimum log level
     */
    void setLogLevel(LogLevel level) noexcept { m_minLevel = level; }

    /**
     * @brief Get the current log level
     */
    [[nodiscard]] LogLevel getLogLevel() const noexcept { return m_minLevel; }

    /**
     * @brief Enable or disable console output
     */
    void setConsoleOutput(bool enabled) noexcept { m_consoleOutput = enabled; }

    /**
     * @brief Log a general message
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief Log a file operation
     */
    void logOperation(const OperationRecord& record);

    /**
     * @brief Log scan results
     */
    void logScan(const fs::path& path, std::size_t filesFound);

    /**
     * @brief Log organization statistics
     */
    void logStats(const OrganizationStats& stats);

    // Convenience methods
    void debug(const std::string& message) { log(LogLevel::Debug, message); }
    void info(const std::string& message) { log(LogLevel::Info, message); }
    void warning(const std::string& message) { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error, message); }

    /**
     * @brief Get all operation records
     */
    [[nodiscard]] std::vector<OperationRecord> getOperationHistory() const;

    /**
     * @brief Clear operation history
     */
    void clearHistory();

private:
    Logger() = default;
    ~Logger();

    std::string getCurrentTimestamp() const;
    void writeJsonLine(const std::string& json);

    mutable std::mutex m_mutex;
    std::ofstream m_file;
    fs::path m_logPath;
    bool m_initialized = false;
    bool m_consoleOutput = true;
    LogLevel m_minLevel = LogLevel::Info;
    std::vector<OperationRecord> m_operationHistory;
};

// Convenience macros
#define FF_LOG_DEBUG(msg) fileforge::Logger::instance().debug(msg)
#define FF_LOG_INFO(msg) fileforge::Logger::instance().info(msg)
#define FF_LOG_WARNING(msg) fileforge::Logger::instance().warning(msg)
#define FF_LOG_ERROR(msg) fileforge::Logger::instance().error(msg)

} // namespace fileforge
