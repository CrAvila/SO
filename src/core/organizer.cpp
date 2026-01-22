/**
 * @file organizer.cpp
 * @brief Main file organization engine implementation
 */

#include <fileforge/organizer.hpp>
#include <fileforge/logger.hpp>

#include <algorithm>
#include <chrono>
#include <future>
#include <thread>

namespace fileforge {

Organizer::Organizer(const Config& config)
    : m_config(config)
    , m_scanner(std::make_unique<Scanner>(config))
    , m_categorizer(std::make_unique<Categorizer>(config))
    , m_renamer(std::make_unique<Renamer>(config)) {}

bool Organizer::validateSourcePath(const fs::path& path) const {
    if (!fs::exists(path)) {
        FF_LOG_ERROR("Source path does not exist: " + path.string());
        return false;
    }

    if (!fs::is_directory(path)) {
        FF_LOG_ERROR("Source path is not a directory: " + path.string());
        return false;
    }

    return true;
}

bool Organizer::createDirectoryStructure(const fs::path& basePath) {
    try {
        // Create category directories
        const std::vector<FileCategory> categories = {
            FileCategory::Images,
            FileCategory::Documents,
            FileCategory::Spreadsheets,
            FileCategory::Presentations,
            FileCategory::Videos,
            FileCategory::Audio,
            FileCategory::Archives,
            FileCategory::Code,
            FileCategory::Data,
            FileCategory::Executables,
            FileCategory::Other
        };

        for (auto category : categories) {
            fs::path categoryPath = basePath / categoryToString(category);

            if (m_config.dryRun()) {
                FF_LOG_INFO("[DRY RUN] Would create: " + categoryPath.string());
            } else {
                std::error_code ec;
                fs::create_directories(categoryPath, ec);

                if (ec) {
                    FF_LOG_ERROR("Failed to create directory: " + categoryPath.string() +
                                " - " + ec.message());
                    return false;
                }
            }
        }

        return true;
    } catch (const std::exception& e) {
        FF_LOG_ERROR("Error creating directory structure: " + std::string(e.what()));
        return false;
    }
}

OperationRecord Organizer::moveFile(const FileInfo& file,
                                    const fs::path& destinationDir,
                                    const std::string& newName) {
    OperationRecord record;
    record.action = "move";
    record.sourcePath = file.path;
    record.oldName = file.name;
    record.newName = newName;

    fs::path destPath = destinationDir / newName;
    record.destinationPath = destPath;

    if (m_config.dryRun()) {
        record.status = OperationStatus::DryRun;
        FF_LOG_INFO("[DRY RUN] " + file.path.string() + " -> " + destPath.string());
        return record;
    }

    try {
        // Check if source exists
        if (!fs::exists(file.path)) {
            record.status = OperationStatus::Failed;
            record.errorMessage = "Source file not found";
            FF_LOG_ERROR("Source file not found: " + file.path.string());
            return record;
        }

        // Check if destination already exists
        if (fs::exists(destPath)) {
            record.status = OperationStatus::Skipped;
            record.errorMessage = "Destination already exists";
            FF_LOG_WARNING("Destination exists, skipping: " + destPath.string());
            return record;
        }

        // Perform the move
        std::error_code ec;
        fs::rename(file.path, destPath, ec);

        if (ec) {
            // Try copy + delete if rename fails (cross-filesystem)
            fs::copy_file(file.path, destPath, fs::copy_options::none, ec);
            if (!ec) {
                fs::remove(file.path, ec);
            }
        }

        if (ec) {
            record.status = OperationStatus::Failed;
            record.errorMessage = ec.message();
            FF_LOG_ERROR("Failed to move: " + file.path.string() + " - " + ec.message());
        } else {
            record.status = OperationStatus::Success;
            FF_LOG_INFO("Moved: " + file.path.string() + " -> " + destPath.string());
        }
    } catch (const std::exception& e) {
        record.status = OperationStatus::Failed;
        record.errorMessage = e.what();
        FF_LOG_ERROR("Exception moving file: " + std::string(e.what()));
    }

    return record;
}

OperationRecord Organizer::processFile(const FileInfo& file,
                                       const fs::path& destinationBase) {
    // Get category directory
    fs::path categoryDir = destinationBase / categoryToString(file.category);

    // Generate new name if renaming is enabled
    std::string newName;
    if (m_config.enableRenaming()) {
        newName = m_renamer->generateName(file, categoryDir);
    } else {
        newName = file.name;

        // Handle collision without renaming
        if (fs::exists(categoryDir / newName)) {
            int counter = 1;
            auto dotPos = newName.rfind('.');
            std::string baseName = (dotPos != std::string::npos)
                ? newName.substr(0, dotPos) : newName;
            std::string ext = (dotPos != std::string::npos)
                ? newName.substr(dotPos) : "";

            do {
                newName = baseName + "_" + std::to_string(counter++) + ext;
            } while (fs::exists(categoryDir / newName) && counter < 10000);
        }
    }

    return moveFile(file, categoryDir, newName);
}

void Organizer::processFilesParallel(const std::vector<FileInfo>& files,
                                     const fs::path& destinationBase,
                                     OrganizationStats& stats) {
    std::size_t numThreads = std::min(
        m_config.parallelThreads(),
        static_cast<std::size_t>(std::thread::hardware_concurrency())
    );

    if (numThreads <= 1 || files.size() < 10) {
        // Process sequentially for small batches
        for (std::size_t i = 0; i < files.size(); ++i) {
            if (m_cancelled.load()) break;

            auto record = processFile(files[i], destinationBase);
            m_operationHistory.push_back(record);

            switch (record.status) {
                case OperationStatus::Success:
                    ++stats.filesProcessed;
                    ++stats.filesMoved;
                    if (record.oldName != record.newName) {
                        ++stats.filesRenamed;
                    }
                    break;
                case OperationStatus::Skipped:
                    ++stats.filesSkipped;
                    break;
                case OperationStatus::Failed:
                    ++stats.filesFailed;
                    break;
                case OperationStatus::DryRun:
                    ++stats.filesProcessed;
                    break;
            }

            reportProgress(i + 1, files.size(), files[i].name);
            Logger::instance().logOperation(record);
        }
        return;
    }

    // Parallel processing
    std::vector<std::future<std::vector<OperationRecord>>> futures;
    std::size_t batchSize = (files.size() + numThreads - 1) / numThreads;

    for (std::size_t t = 0; t < numThreads; ++t) {
        std::size_t start = t * batchSize;
        std::size_t end = std::min(start + batchSize, files.size());

        if (start >= files.size()) break;

        futures.push_back(std::async(std::launch::async, [&, start, end]() {
            std::vector<OperationRecord> records;

            for (std::size_t i = start; i < end; ++i) {
                if (m_cancelled.load()) break;
                records.push_back(processFile(files[i], destinationBase));
            }

            return records;
        }));
    }

    // Collect results
    std::size_t processed = 0;
    for (auto& future : futures) {
        auto records = future.get();

        for (auto& record : records) {
            m_operationHistory.push_back(record);

            switch (record.status) {
                case OperationStatus::Success:
                    ++stats.filesProcessed;
                    ++stats.filesMoved;
                    if (record.oldName != record.newName) {
                        ++stats.filesRenamed;
                    }
                    break;
                case OperationStatus::Skipped:
                    ++stats.filesSkipped;
                    break;
                case OperationStatus::Failed:
                    ++stats.filesFailed;
                    break;
                case OperationStatus::DryRun:
                    ++stats.filesProcessed;
                    break;
            }

            Logger::instance().logOperation(record);
            ++processed;
            reportProgress(processed, files.size(), record.oldName);
        }
    }
}

void Organizer::reportProgress(std::size_t current, std::size_t total,
                               const std::string& currentFile) {
    if (m_progressCallback) {
        m_progressCallback(current, total, currentFile);
    }
}

OrganizationStats Organizer::organize(const fs::path& sourcePath) {
    fs::path destinationPath = sourcePath;

    if (m_config.createOrganizedFolder()) {
        destinationPath = sourcePath / m_config.organizedFolderName();
    }

    return organize(sourcePath, destinationPath);
}

OrganizationStats Organizer::organize(const fs::path& sourcePath,
                                       const fs::path& destinationPath) {
    auto startTime = std::chrono::steady_clock::now();

    m_cancelled.store(false);
    m_operationHistory.clear();
    m_renamer->reset();

    OrganizationStats stats;

    // Validate source
    if (!validateSourcePath(sourcePath)) {
        return stats;
    }

    FF_LOG_INFO("Starting organization: " + sourcePath.string() + " -> " + destinationPath.string());

    // Scan for files
    std::vector<FileInfo> files;
    if (m_config.parallelThreads() > 1) {
        files = m_scanner->scanParallel(sourcePath);
    } else {
        files = m_scanner->scan(sourcePath);
    }

    if (m_cancelled.load()) {
        FF_LOG_WARNING("Operation cancelled during scan");
        return stats;
    }

    stats.totalFiles = files.size();

    if (files.empty()) {
        FF_LOG_INFO("No files found to organize");
        return stats;
    }

    // Categorize files
    m_categorizer->categorizeAll(files);

    // Calculate total bytes
    for (const auto& file : files) {
        stats.totalBytes += file.size;
    }

    // Create directory structure
    if (!createDirectoryStructure(destinationPath)) {
        FF_LOG_ERROR("Failed to create directory structure");
        return stats;
    }

    // Process files
    processFilesParallel(files, destinationPath, stats);

    // Store for undo
    m_undoHistory = m_operationHistory;

    // Calculate duration
    auto endTime = std::chrono::steady_clock::now();
    stats.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Log final stats
    m_lastStats = stats;
    Logger::instance().logStats(stats);

    FF_LOG_INFO("Organization complete: " + std::to_string(stats.filesProcessed) +
               "/" + std::to_string(stats.totalFiles) + " files processed in " +
               std::to_string(stats.duration.count()) + "ms");

    return stats;
}

std::vector<OperationRecord> Organizer::preview(const fs::path& sourcePath) {
    // Temporarily enable dry run
    bool wasDryRun = m_config.dryRun();
    m_config.setDryRun(true);

    fs::path destinationPath = sourcePath;
    if (m_config.createOrganizedFolder()) {
        destinationPath = sourcePath / m_config.organizedFolderName();
    }

    organize(sourcePath, destinationPath);

    // Restore dry run setting
    m_config.setDryRun(wasDryRun);

    return m_operationHistory;
}

bool Organizer::undo() {
    if (m_undoHistory.empty()) {
        FF_LOG_WARNING("Nothing to undo");
        return false;
    }

    FF_LOG_INFO("Starting undo operation");

    bool allSuccess = true;

    // Reverse the operations
    for (auto it = m_undoHistory.rbegin(); it != m_undoHistory.rend(); ++it) {
        if (it->status != OperationStatus::Success) {
            continue; // Skip failed operations
        }

        try {
            std::error_code ec;

            // Move file back
            if (fs::exists(it->destinationPath)) {
                fs::rename(it->destinationPath, it->sourcePath, ec);

                if (ec) {
                    // Try copy + delete
                    fs::copy_file(it->destinationPath, it->sourcePath,
                                 fs::copy_options::overwrite_existing, ec);
                    if (!ec) {
                        fs::remove(it->destinationPath, ec);
                    }
                }

                if (ec) {
                    FF_LOG_ERROR("Undo failed for: " + it->destinationPath.string());
                    allSuccess = false;
                } else {
                    FF_LOG_INFO("Undone: " + it->destinationPath.string() +
                               " -> " + it->sourcePath.string());
                }
            }
        } catch (const std::exception& e) {
            FF_LOG_ERROR("Undo exception: " + std::string(e.what()));
            allSuccess = false;
        }
    }

    if (allSuccess) {
        m_undoHistory.clear();
        FF_LOG_INFO("Undo complete");
    }

    return allSuccess;
}

} // namespace fileforge
