#pragma once

/**
 * @file scanner.hpp
 * @brief File system scanner with parallelization support
 */

#include <atomic>
#include <filesystem>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

#include "config.hpp"
#include "types.hpp"

namespace fileforge {

/**
 * @brief Callback for progress reporting
 * @param current Current file being processed
 * @param total Total files found so far
 */
using ScanProgressCallback = std::function<void(std::size_t current, std::size_t total)>;

/**
 * @brief High-performance file system scanner
 */
class Scanner {
public:
    /**
     * @brief Construct scanner with configuration
     * @param config Configuration options
     */
    explicit Scanner(const Config& config);

    /**
     * @brief Scan a directory for files
     * @param path Directory to scan
     * @return Vector of FileInfo for all found files
     */
    [[nodiscard]] std::vector<FileInfo> scan(const fs::path& path);

    /**
     * @brief Scan a directory with parallelization
     * @param path Directory to scan
     * @param numThreads Number of worker threads (0 = auto)
     * @return Vector of FileInfo for all found files
     */
    [[nodiscard]] std::vector<FileInfo> scanParallel(const fs::path& path,
                                                      std::size_t numThreads = 0);

    /**
     * @brief Set progress callback
     * @param callback Function to call on progress updates
     */
    void setProgressCallback(ScanProgressCallback callback) {
        m_progressCallback = std::move(callback);
    }

    /**
     * @brief Cancel ongoing scan operation
     */
    void cancel() noexcept { m_cancelled.store(true); }

    /**
     * @brief Check if scan was cancelled
     */
    [[nodiscard]] bool isCancelled() const noexcept { return m_cancelled.load(); }

    /**
     * @brief Reset scanner state for new scan
     */
    void reset() noexcept { m_cancelled.store(false); }

    /**
     * @brief Get statistics from last scan
     */
    [[nodiscard]] std::size_t getLastScanCount() const noexcept { return m_lastScanCount; }

private:
    const Config& m_config;
    ScanProgressCallback m_progressCallback;
    std::atomic<bool> m_cancelled{false};
    std::atomic<std::size_t> m_lastScanCount{0};
    mutable std::mutex m_mutex;

    /**
     * @brief Check if a file should be included based on config
     */
    [[nodiscard]] bool shouldIncludeFile(const fs::directory_entry& entry) const;

    /**
     * @brief Process a single directory entry
     */
    [[nodiscard]] std::optional<FileInfo> processEntry(const fs::directory_entry& entry) const;

    /**
     * @brief Worker function for parallel scanning
     */
    void scanWorker(std::queue<fs::path>& directories,
                    std::vector<FileInfo>& results,
                    std::mutex& queueMutex,
                    std::mutex& resultsMutex,
                    std::atomic<bool>& done);
};

} // namespace fileforge
