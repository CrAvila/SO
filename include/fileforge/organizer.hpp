#pragma once

/**
 * @file organizer.hpp
 * @brief Main file organization engine
 */

#include <atomic>
#include <filesystem>
#include <functional>
#include <future>
#include <memory>
#include <vector>

#include "categorizer.hpp"
#include "config.hpp"
#include "renamer.hpp"
#include "scanner.hpp"
#include "types.hpp"

namespace fileforge {

/**
 * @brief Progress callback for organization
 * @param current Current file index
 * @param total Total files
 * @param currentFile Current file being processed
 */
using OrganizeProgressCallback = std::function<void(
    std::size_t current,
    std::size_t total,
    const std::string& currentFile)>;

/**
 * @brief Main file organization engine
 *
 * Orchestrates scanning, categorization, renaming, and file movement
 * with parallelization support and comprehensive logging.
 */
class Organizer {
public:
    /**
     * @brief Construct organizer with configuration
     * @param config Configuration options
     */
    explicit Organizer(const Config& config);

    /**
     * @brief Organize files in a directory
     * @param sourcePath Directory to organize
     * @return Organization statistics
     */
    [[nodiscard]] OrganizationStats organize(const fs::path& sourcePath);

    /**
     * @brief Organize files with a custom destination
     * @param sourcePath Directory containing files
     * @param destinationPath Where to put organized files
     * @return Organization statistics
     */
    [[nodiscard]] OrganizationStats organize(const fs::path& sourcePath,
                                              const fs::path& destinationPath);

    /**
     * @brief Preview organization without making changes
     * @param sourcePath Directory to preview
     * @return Vector of planned operations
     */
    [[nodiscard]] std::vector<OperationRecord> preview(const fs::path& sourcePath);

    /**
     * @brief Undo the last organization operation
     * @return true if undo successful
     */
    bool undo();

    /**
     * @brief Set progress callback
     */
    void setProgressCallback(OrganizeProgressCallback callback) {
        m_progressCallback = std::move(callback);
    }

    /**
     * @brief Cancel ongoing operation
     */
    void cancel() noexcept { m_cancelled.store(true); }

    /**
     * @brief Check if operation was cancelled
     */
    [[nodiscard]] bool isCancelled() const noexcept { return m_cancelled.load(); }

    /**
     * @brief Get the last operation statistics
     */
    [[nodiscard]] const OrganizationStats& getLastStats() const noexcept {
        return m_lastStats;
    }

    /**
     * @brief Get operation history
     */
    [[nodiscard]] const std::vector<OperationRecord>& getOperationHistory() const noexcept {
        return m_operationHistory;
    }

private:
    Config m_config;
    std::unique_ptr<Scanner> m_scanner;
    std::unique_ptr<Categorizer> m_categorizer;
    std::unique_ptr<Renamer> m_renamer;

    OrganizeProgressCallback m_progressCallback;
    std::atomic<bool> m_cancelled{false};
    OrganizationStats m_lastStats;
    std::vector<OperationRecord> m_operationHistory;
    std::vector<OperationRecord> m_undoHistory;

    /**
     * @brief Create destination directory structure
     */
    bool createDirectoryStructure(const fs::path& basePath);

    /**
     * @brief Move a single file with error handling
     */
    OperationRecord moveFile(const FileInfo& file,
                             const fs::path& destinationDir,
                             const std::string& newName);

    /**
     * @brief Process files in parallel
     */
    void processFilesParallel(const std::vector<FileInfo>& files,
                              const fs::path& destinationBase,
                              OrganizationStats& stats);

    /**
     * @brief Process a single file
     */
    OperationRecord processFile(const FileInfo& file,
                                const fs::path& destinationBase);

    /**
     * @brief Report progress if callback is set
     */
    void reportProgress(std::size_t current, std::size_t total,
                        const std::string& currentFile);

    /**
     * @brief Validate source path
     */
    [[nodiscard]] bool validateSourcePath(const fs::path& path) const;
};

} // namespace fileforge
