/**
 * @file categorizer.cpp
 * @brief File categorization implementation
 */

#include <fileforge/categorizer.hpp>

#include <algorithm>
#include <cctype>

namespace fileforge {

Categorizer::Categorizer(const Config& config)
    : m_config(config) {
    buildExtensionMap();
}

void Categorizer::buildExtensionMap() {
    m_extensionMap.clear();

    const auto& categoryMap = m_config.getCategoryMap();
    for (const auto& [category, extensions] : categoryMap) {
        for (const auto& ext : extensions) {
            auto normalizedExt = normalizeExtension(ext);
            m_extensionMap[normalizedExt] = category;
        }
    }
}

std::string Categorizer::normalizeExtension(std::string_view ext) {
    std::string result;
    result.reserve(ext.size() + 1);

    // Ensure it starts with a dot
    if (ext.empty() || ext[0] != '.') {
        result = '.';
    }

    // Convert to lowercase
    for (char c : ext) {
        result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    return result;
}

FileCategory Categorizer::getCategory(std::string_view extension) const {
    auto normalized = normalizeExtension(extension);
    auto it = m_extensionMap.find(normalized);

    if (it != m_extensionMap.end()) {
        return it->second;
    }

    return FileCategory::Other;
}

void Categorizer::categorize(FileInfo& fileInfo) const {
    fileInfo.category = getCategory(fileInfo.extension);
}

void Categorizer::categorizeAll(std::vector<FileInfo>& files) const {
    for (auto& file : files) {
        categorize(file);
    }
}

bool Categorizer::isKnownExtension(std::string_view extension) const {
    auto normalized = normalizeExtension(extension);
    return m_extensionMap.find(normalized) != m_extensionMap.end();
}

std::vector<std::string> Categorizer::getExtensions(FileCategory category) const {
    return m_config.getExtensions(category);
}

std::map<FileCategory, std::vector<FileInfo>>
Categorizer::groupByCategory(const std::vector<FileInfo>& files) const {
    std::map<FileCategory, std::vector<FileInfo>> grouped;

    for (const auto& file : files) {
        grouped[file.category].push_back(file);
    }

    return grouped;
}

std::map<FileCategory, std::size_t>
Categorizer::getCategoryStats(const std::vector<FileInfo>& files) const {
    std::map<FileCategory, std::size_t> stats;

    // Initialize all categories to 0
    stats[FileCategory::Images] = 0;
    stats[FileCategory::Documents] = 0;
    stats[FileCategory::Spreadsheets] = 0;
    stats[FileCategory::Presentations] = 0;
    stats[FileCategory::Videos] = 0;
    stats[FileCategory::Audio] = 0;
    stats[FileCategory::Archives] = 0;
    stats[FileCategory::Code] = 0;
    stats[FileCategory::Data] = 0;
    stats[FileCategory::Executables] = 0;
    stats[FileCategory::Other] = 0;

    for (const auto& file : files) {
        ++stats[file.category];
    }

    return stats;
}

} // namespace fileforge
