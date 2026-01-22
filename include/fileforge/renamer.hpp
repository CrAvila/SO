#pragma once

/**
 * @file renamer.hpp
 * @brief Intelligent file renaming system
 */

#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>

#include "config.hpp"
#include "types.hpp"

namespace fileforge {

/**
 * @brief Intelligent file renamer with collision handling
 */
class Renamer {
public:
    /**
     * @brief Construct renamer with configuration
     * @param config Configuration containing rename options
     */
    explicit Renamer(const Config& config);

    /**
     * @brief Generate a new name for a file
     * @param fileInfo File information
     * @param destinationDir Destination directory for collision checking
     * @return New file name (without path)
     */
    [[nodiscard]] std::string generateName(const FileInfo& fileInfo,
                                           const fs::path& destinationDir);

    /**
     * @brief Generate names for multiple files
     * @param files Files to rename
     * @param destinationDir Destination directory
     * @return Map of original path to new name
     */
    [[nodiscard]] std::map<fs::path, std::string> generateNames(
        const std::vector<FileInfo>& files,
        const fs::path& destinationDir);

    /**
     * @brief Check if a name would be a collision
     * @param name Proposed file name
     * @param directory Directory to check
     * @return true if name already exists
     */
    [[nodiscard]] bool wouldCollide(const std::string& name,
                                    const fs::path& directory) const;

    /**
     * @brief Reset the renamer state (clear used names cache)
     */
    void reset();

    /**
     * @brief Extract descriptor from original filename
     * @param filename Original filename
     * @return Extracted descriptor or default
     */
    [[nodiscard]] static std::string extractDescriptor(const std::string& filename);

    /**
     * @brief Sanitize a filename for safe filesystem use
     * @param filename Filename to sanitize
     * @return Sanitized filename
     */
    [[nodiscard]] static std::string sanitizeFilename(const std::string& filename);

private:
    const Config& m_config;
    std::unordered_set<std::string> m_usedNames;
    std::map<std::string, int> m_versionCounters;
    mutable std::mutex m_mutex;

    /**
     * @brief Generate date string from file info
     */
    [[nodiscard]] std::string generateDateString(const FileInfo& fileInfo) const;

    /**
     * @brief Get next available version number
     */
    [[nodiscard]] int getNextVersion(const std::string& baseName);

    /**
     * @brief Build the final filename from components
     */
    [[nodiscard]] std::string buildFilename(const std::string& category,
                                            const std::string& date,
                                            const std::string& descriptor,
                                            int version,
                                            const std::string& extension) const;
};

} // namespace fileforge
