#pragma once

/**
 * @file types.hpp
 * @brief Common types and structures for FileForge
 */

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fileforge {

namespace fs = std::filesystem;

/**
 * @brief File category enumeration
 */
enum class FileCategory {
    Images,
    Documents,
    Spreadsheets,
    Presentations,
    Videos,
    Audio,
    Archives,
    Code,
    Data,
    Executables,
    Other
};

/**
 * @brief Convert FileCategory to string
 */
constexpr const char* categoryToString(FileCategory category) noexcept {
    switch (category) {
        case FileCategory::Images:        return "Images";
        case FileCategory::Documents:     return "Documents";
        case FileCategory::Spreadsheets:  return "Spreadsheets";
        case FileCategory::Presentations: return "Presentations";
        case FileCategory::Videos:        return "Videos";
        case FileCategory::Audio:         return "Audio";
        case FileCategory::Archives:      return "Archives";
        case FileCategory::Code:          return "Code";
        case FileCategory::Data:          return "Data";
        case FileCategory::Executables:   return "Executables";
        case FileCategory::Other:         return "Other";
    }
    return "Unknown";
}

/**
 * @brief Parse string to FileCategory
 */
inline std::optional<FileCategory> stringToCategory(std::string_view str) noexcept {
    if (str == "Images")        return FileCategory::Images;
    if (str == "Documents")     return FileCategory::Documents;
    if (str == "Spreadsheets")  return FileCategory::Spreadsheets;
    if (str == "Presentations") return FileCategory::Presentations;
    if (str == "Videos")        return FileCategory::Videos;
    if (str == "Audio")         return FileCategory::Audio;
    if (str == "Archives")      return FileCategory::Archives;
    if (str == "Code")          return FileCategory::Code;
    if (str == "Data")          return FileCategory::Data;
    if (str == "Executables")   return FileCategory::Executables;
    if (str == "Other")         return FileCategory::Other;
    return std::nullopt;
}

/**
 * @brief Information about a single file
 */
struct FileInfo {
    fs::path path;                              ///< Full path to the file
    std::string name;                           ///< File name with extension
    std::string extension;                      ///< File extension (lowercase, with dot)
    std::uintmax_t size;                        ///< File size in bytes
    fs::file_time_type lastModified;            ///< Last modification time
    FileCategory category;                      ///< Detected category

    FileInfo() = default;

    explicit FileInfo(const fs::path& filePath)
        : path(filePath)
        , name(filePath.filename().string())
        , extension(filePath.extension().string())
        , size(0)
        , category(FileCategory::Other) {
        // Convert extension to lowercase
        for (auto& c : extension) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
};

/**
 * @brief Result of a file operation
 */
enum class OperationStatus {
    Success,
    Failed,
    Skipped,
    DryRun
};

/**
 * @brief Convert OperationStatus to string
 */
constexpr const char* statusToString(OperationStatus status) noexcept {
    switch (status) {
        case OperationStatus::Success: return "success";
        case OperationStatus::Failed:  return "failed";
        case OperationStatus::Skipped: return "skipped";
        case OperationStatus::DryRun:  return "dry_run";
    }
    return "unknown";
}

/**
 * @brief Record of a single file operation
 */
struct OperationRecord {
    std::string action;                         ///< Action type (move, rename, copy)
    fs::path sourcePath;                        ///< Original file path
    fs::path destinationPath;                   ///< New file path
    std::string oldName;                        ///< Original file name
    std::string newName;                        ///< New file name (if renamed)
    OperationStatus status;                     ///< Operation result
    std::string errorMessage;                   ///< Error message if failed
    std::chrono::system_clock::time_point timestamp; ///< When operation occurred

    OperationRecord()
        : status(OperationStatus::Success)
        , timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @brief Statistics for an organization session
 */
struct OrganizationStats {
    std::size_t totalFiles = 0;                 ///< Total files found
    std::size_t filesProcessed = 0;             ///< Files successfully processed
    std::size_t filesMoved = 0;                 ///< Files moved to new location
    std::size_t filesRenamed = 0;               ///< Files renamed
    std::size_t filesSkipped = 0;               ///< Files skipped
    std::size_t filesFailed = 0;                ///< Files that failed to process
    std::uintmax_t totalBytes = 0;              ///< Total bytes processed
    std::chrono::milliseconds duration{0};      ///< Total processing time

    /**
     * @brief Calculate success rate as percentage
     */
    [[nodiscard]] double successRate() const noexcept {
        if (totalFiles == 0) return 100.0;
        return (static_cast<double>(filesProcessed) / static_cast<double>(totalFiles)) * 100.0;
    }
};

/**
 * @brief Rename pattern components
 */
struct RenamePattern {
    bool includeCategory = true;                ///< Include category in name
    bool includeDate = true;                    ///< Include date in name
    bool includeDescriptor = true;              ///< Include descriptor in name
    bool includeVersion = true;                 ///< Include version number
    std::string separator = "_";                ///< Separator between components
    std::string dateFormat = "%Y-%m-%d";        ///< Date format string
};

} // namespace fileforge
