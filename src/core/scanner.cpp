/**
 * @file scanner.cpp
 * @brief File system scanner implementation with parallelization
 */

#include <fileforge/scanner.hpp>
#include <fileforge/logger.hpp>

#include <algorithm>
#include <thread>

namespace fileforge {

Scanner::Scanner(const Config& config)
    : m_config(config) {}

bool Scanner::shouldIncludeFile(const fs::directory_entry& entry) const {
    // Skip directories
    if (!entry.is_regular_file()) {
        return false;
    }

    // Skip hidden files unless configured
    const auto filename = entry.path().filename().string();
    if (!m_config.includeHidden() && !filename.empty() && filename[0] == '.') {
        return false;
    }

    return true;
}

std::optional<FileInfo> Scanner::processEntry(const fs::directory_entry& entry) const {
    try {
        if (!shouldIncludeFile(entry)) {
            return std::nullopt;
        }

        FileInfo info(entry.path());

        // Get file size
        std::error_code ec;
        info.size = entry.file_size(ec);
        if (ec) {
            info.size = 0;
        }

        // Get last modified time
        info.lastModified = entry.last_write_time(ec);

        return info;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::vector<FileInfo> Scanner::scan(const fs::path& path) {
    std::vector<FileInfo> results;
    m_cancelled.store(false);
    m_lastScanCount.store(0);

    if (!fs::exists(path) || !fs::is_directory(path)) {
        FF_LOG_ERROR("Invalid directory: " + path.string());
        return results;
    }

    FF_LOG_INFO("Scanning directory: " + path.string());

    try {
        auto options = fs::directory_options::skip_permission_denied;

        if (m_config.recursive()) {
            for (const auto& entry : fs::recursive_directory_iterator(path, options)) {
                if (m_cancelled.load()) {
                    FF_LOG_WARNING("Scan cancelled");
                    break;
                }

                if (auto fileInfo = processEntry(entry)) {
                    results.push_back(std::move(*fileInfo));
                    m_lastScanCount.store(results.size());

                    if (m_progressCallback) {
                        m_progressCallback(results.size(), 0);
                    }
                }
            }
        } else {
            for (const auto& entry : fs::directory_iterator(path, options)) {
                if (m_cancelled.load()) {
                    FF_LOG_WARNING("Scan cancelled");
                    break;
                }

                if (auto fileInfo = processEntry(entry)) {
                    results.push_back(std::move(*fileInfo));
                    m_lastScanCount.store(results.size());

                    if (m_progressCallback) {
                        m_progressCallback(results.size(), 0);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        FF_LOG_ERROR("Scan error: " + std::string(e.what()));
    }

    Logger::instance().logScan(path, results.size());
    FF_LOG_INFO("Scan complete: " + std::to_string(results.size()) + " files found");

    return results;
}

std::vector<FileInfo> Scanner::scanParallel(const fs::path& path, std::size_t numThreads) {
    m_cancelled.store(false);
    m_lastScanCount.store(0);

    if (!fs::exists(path) || !fs::is_directory(path)) {
        FF_LOG_ERROR("Invalid directory: " + path.string());
        return {};
    }

    // Determine thread count
    if (numThreads == 0) {
        numThreads = std::max(1u, std::thread::hardware_concurrency());
    }
    numThreads = std::min(numThreads, m_config.parallelThreads());

    FF_LOG_INFO("Parallel scan with " + std::to_string(numThreads) + " threads: " + path.string());

    // For non-recursive scans, parallel doesn't help much
    if (!m_config.recursive()) {
        return scan(path);
    }

    // Collect all subdirectories first
    std::queue<fs::path> directories;
    std::vector<FileInfo> results;
    std::mutex queueMutex;
    std::mutex resultsMutex;
    std::atomic<bool> done{false};

    // Add root directory
    directories.push(path);

    // Process root level to find subdirectories
    try {
        auto options = fs::directory_options::skip_permission_denied;
        for (const auto& entry : fs::directory_iterator(path, options)) {
            if (entry.is_directory()) {
                directories.push(entry.path());
            } else if (auto fileInfo = processEntry(entry)) {
                results.push_back(std::move(*fileInfo));
            }
        }
    } catch (const std::exception& e) {
        FF_LOG_WARNING("Error scanning root: " + std::string(e.what()));
    }

    // If few directories, just use single-threaded
    if (directories.size() <= 2) {
        return scan(path);
    }

    // Launch worker threads
    std::vector<std::thread> workers;
    workers.reserve(numThreads);

    for (std::size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&Scanner::scanWorker, this,
                            std::ref(directories),
                            std::ref(results),
                            std::ref(queueMutex),
                            std::ref(resultsMutex),
                            std::ref(done));
    }

    // Wait for all workers
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    Logger::instance().logScan(path, results.size());
    FF_LOG_INFO("Parallel scan complete: " + std::to_string(results.size()) + " files found");

    return results;
}

void Scanner::scanWorker(std::queue<fs::path>& directories,
                         std::vector<FileInfo>& results,
                         std::mutex& queueMutex,
                         std::mutex& resultsMutex,
                         std::atomic<bool>& done) {
    auto options = fs::directory_options::skip_permission_denied;

    while (!m_cancelled.load()) {
        fs::path currentDir;

        // Get next directory from queue
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (directories.empty()) {
                if (done.load()) {
                    return;
                }
                // Queue empty but not done, wait a bit
                std::this_thread::yield();
                continue;
            }
            currentDir = directories.front();
            directories.pop();
        }

        // Process directory
        try {
            std::vector<FileInfo> localResults;
            std::vector<fs::path> subdirs;

            for (const auto& entry : fs::directory_iterator(currentDir, options)) {
                if (m_cancelled.load()) {
                    return;
                }

                if (entry.is_directory()) {
                    subdirs.push_back(entry.path());
                } else if (auto fileInfo = processEntry(entry)) {
                    localResults.push_back(std::move(*fileInfo));
                }
            }

            // Add subdirectories to queue
            if (!subdirs.empty()) {
                std::lock_guard<std::mutex> lock(queueMutex);
                for (auto& subdir : subdirs) {
                    directories.push(std::move(subdir));
                }
            }

            // Add results
            if (!localResults.empty()) {
                std::lock_guard<std::mutex> lock(resultsMutex);
                results.insert(results.end(),
                              std::make_move_iterator(localResults.begin()),
                              std::make_move_iterator(localResults.end()));
                m_lastScanCount.store(results.size());

                if (m_progressCallback) {
                    m_progressCallback(results.size(), 0);
                }
            }
        } catch (const std::exception&) {
            // Continue with next directory
        }
    }
}

} // namespace fileforge
