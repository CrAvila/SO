/**
 * @file logger.cpp
 * @brief Thread-safe logging implementation
 */

#include <fileforge/logger.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

namespace fileforge {

using json = nlohmann::json;

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    close();
}

bool Logger::initialize(const fs::path& logPath, bool consoleOutput) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_initialized) {
        close();
    }

    m_logPath = logPath;
    m_consoleOutput = consoleOutput;

    // Create parent directories if needed
    if (logPath.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(logPath.parent_path(), ec);
    }

    m_file.open(logPath, std::ios::out | std::ios::app);
    if (!m_file.is_open()) {
        return false;
    }

    m_initialized = true;

    // Log initialization
    json initLog;
    initLog["timestamp"] = getCurrentTimestamp();
    initLog["level"] = "INFO";
    initLog["message"] = "FileForge logger initialized";
    initLog["version"] = "1.0.0";
    writeJsonLine(initLog.dump());

    return true;
}

void Logger::close() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_file.is_open()) {
        m_file.flush();
        m_file.close();
    }
    m_initialized = false;
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    return oss.str();
}

void Logger::writeJsonLine(const std::string& jsonStr) {
    if (m_initialized && m_file.is_open()) {
        m_file << jsonStr << '\n';
        m_file.flush();
    }

    if (m_consoleOutput) {
        std::cout << jsonStr << '\n';
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < m_minLevel) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    json logEntry;
    logEntry["timestamp"] = getCurrentTimestamp();
    logEntry["level"] = logLevelToString(level);
    logEntry["message"] = message;

    writeJsonLine(logEntry.dump());
}

void Logger::logOperation(const OperationRecord& record) {
    std::lock_guard<std::mutex> lock(m_mutex);

    json logEntry;
    logEntry["timestamp"] = getCurrentTimestamp();
    logEntry["level"] = "INFO";
    logEntry["action"] = record.action;
    logEntry["source"] = record.sourcePath.string();
    logEntry["destination"] = record.destinationPath.string();

    if (!record.oldName.empty()) {
        logEntry["old_name"] = record.oldName;
    }
    if (!record.newName.empty()) {
        logEntry["new_name"] = record.newName;
    }

    logEntry["status"] = statusToString(record.status);

    if (!record.errorMessage.empty()) {
        logEntry["error"] = record.errorMessage;
    }

    writeJsonLine(logEntry.dump());

    // Store in history
    m_operationHistory.push_back(record);
}

void Logger::logScan(const fs::path& path, std::size_t filesFound) {
    std::lock_guard<std::mutex> lock(m_mutex);

    json logEntry;
    logEntry["timestamp"] = getCurrentTimestamp();
    logEntry["level"] = "INFO";
    logEntry["action"] = "scan";
    logEntry["path"] = path.string();
    logEntry["files_found"] = filesFound;

    writeJsonLine(logEntry.dump());
}

void Logger::logStats(const OrganizationStats& stats) {
    std::lock_guard<std::mutex> lock(m_mutex);

    json logEntry;
    logEntry["timestamp"] = getCurrentTimestamp();
    logEntry["level"] = "INFO";
    logEntry["action"] = "summary";
    logEntry["total_files"] = stats.totalFiles;
    logEntry["files_processed"] = stats.filesProcessed;
    logEntry["files_moved"] = stats.filesMoved;
    logEntry["files_renamed"] = stats.filesRenamed;
    logEntry["files_skipped"] = stats.filesSkipped;
    logEntry["files_failed"] = stats.filesFailed;
    logEntry["total_bytes"] = stats.totalBytes;
    logEntry["duration_ms"] = stats.duration.count();
    logEntry["success_rate"] = stats.successRate();

    writeJsonLine(logEntry.dump());
}

std::vector<OperationRecord> Logger::getOperationHistory() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_operationHistory;
}

void Logger::clearHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_operationHistory.clear();
}

} // namespace fileforge
