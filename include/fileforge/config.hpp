#pragma once

/**
 * @file config.hpp
 * @brief Configuration management for FileForge
 */

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "types.hpp"

namespace fileforge {

/**
 * @brief Configuration options for FileForge
 */
class Config {
public:
    /**
     * @brief Default constructor with sensible defaults
     */
    Config();

    /**
     * @brief Load configuration from JSON file
     * @param path Path to configuration file
     * @return true if loaded successfully
     */
    bool loadFromFile(const fs::path& path);

    /**
     * @brief Save configuration to JSON file
     * @param path Path to save configuration
     * @return true if saved successfully
     */
    bool saveToFile(const fs::path& path) const;

    /**
     * @brief Load default configuration
     */
    void loadDefaults();

    // Getters
    [[nodiscard]] bool dryRun() const noexcept { return m_dryRun; }
    [[nodiscard]] bool createOrganizedFolder() const noexcept { return m_createOrganizedFolder; }
    [[nodiscard]] const std::string& organizedFolderName() const noexcept { return m_organizedFolderName; }
    [[nodiscard]] bool enableRenaming() const noexcept { return m_enableRenaming; }
    [[nodiscard]] const RenamePattern& renamePattern() const noexcept { return m_renamePattern; }
    [[nodiscard]] std::size_t parallelThreads() const noexcept { return m_parallelThreads; }
    [[nodiscard]] bool generateLog() const noexcept { return m_generateLog; }
    [[nodiscard]] const std::string& logFile() const noexcept { return m_logFile; }
    [[nodiscard]] bool recursive() const noexcept { return m_recursive; }
    [[nodiscard]] bool includeHidden() const noexcept { return m_includeHidden; }
    [[nodiscard]] bool preserveStructure() const noexcept { return m_preserveStructure; }

    // Setters
    void setDryRun(bool value) noexcept { m_dryRun = value; }
    void setCreateOrganizedFolder(bool value) noexcept { m_createOrganizedFolder = value; }
    void setOrganizedFolderName(const std::string& name) { m_organizedFolderName = name; }
    void setEnableRenaming(bool value) noexcept { m_enableRenaming = value; }
    void setRenamePattern(const RenamePattern& pattern) { m_renamePattern = pattern; }
    void setParallelThreads(std::size_t count) noexcept { m_parallelThreads = count; }
    void setGenerateLog(bool value) noexcept { m_generateLog = value; }
    void setLogFile(const std::string& path) { m_logFile = path; }
    void setRecursive(bool value) noexcept { m_recursive = value; }
    void setIncludeHidden(bool value) noexcept { m_includeHidden = value; }
    void setPreserveStructure(bool value) noexcept { m_preserveStructure = value; }

    /**
     * @brief Get extensions for a category
     */
    [[nodiscard]] const std::vector<std::string>& getExtensions(FileCategory category) const;

    /**
     * @brief Set extensions for a category
     */
    void setExtensions(FileCategory category, const std::vector<std::string>& extensions);

    /**
     * @brief Add extension to a category
     */
    void addExtension(FileCategory category, const std::string& extension);

    /**
     * @brief Get all category mappings
     */
    [[nodiscard]] const std::map<FileCategory, std::vector<std::string>>& getCategoryMap() const noexcept {
        return m_categoryExtensions;
    }

private:
    bool m_dryRun = false;
    bool m_createOrganizedFolder = true;
    std::string m_organizedFolderName = "Organized";
    bool m_enableRenaming = true;
    RenamePattern m_renamePattern;
    std::size_t m_parallelThreads = 4;
    bool m_generateLog = true;
    std::string m_logFile = "fileforge_log.jsonl";
    bool m_recursive = false;
    bool m_includeHidden = false;
    bool m_preserveStructure = false;

    std::map<FileCategory, std::vector<std::string>> m_categoryExtensions;

    void initializeDefaultExtensions();
};

} // namespace fileforge
