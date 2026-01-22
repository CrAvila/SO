#pragma once

/**
 * @file categorizer.hpp
 * @brief File categorization by extension and content
 */

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "config.hpp"
#include "types.hpp"

namespace fileforge {

/**
 * @brief Categorizes files based on their extensions
 */
class Categorizer {
public:
    /**
     * @brief Construct categorizer with configuration
     * @param config Configuration containing extension mappings
     */
    explicit Categorizer(const Config& config);

    /**
     * @brief Categorize a single file
     * @param fileInfo File information (will be updated with category)
     */
    void categorize(FileInfo& fileInfo) const;

    /**
     * @brief Categorize multiple files
     * @param files Vector of files to categorize
     */
    void categorizeAll(std::vector<FileInfo>& files) const;

    /**
     * @brief Get category for an extension
     * @param extension File extension (with or without dot)
     * @return Category for the extension
     */
    [[nodiscard]] FileCategory getCategory(std::string_view extension) const;

    /**
     * @brief Check if extension is known
     * @param extension File extension to check
     * @return true if extension is in a category
     */
    [[nodiscard]] bool isKnownExtension(std::string_view extension) const;

    /**
     * @brief Get all extensions for a category
     * @param category Category to query
     * @return Vector of extensions
     */
    [[nodiscard]] std::vector<std::string> getExtensions(FileCategory category) const;

    /**
     * @brief Group files by category
     * @param files Files to group
     * @return Map of category to files
     */
    [[nodiscard]] std::map<FileCategory, std::vector<FileInfo>>
    groupByCategory(const std::vector<FileInfo>& files) const;

    /**
     * @brief Get statistics about categorization
     * @param files Categorized files
     * @return Map of category to count
     */
    [[nodiscard]] std::map<FileCategory, std::size_t>
    getCategoryStats(const std::vector<FileInfo>& files) const;

private:
    const Config& m_config;
    std::unordered_map<std::string, FileCategory> m_extensionMap;

    /**
     * @brief Build extension lookup map from config
     */
    void buildExtensionMap();

    /**
     * @brief Normalize extension string
     */
    [[nodiscard]] static std::string normalizeExtension(std::string_view ext);
};

} // namespace fileforge
