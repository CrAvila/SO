/**
 * @file renamer.cpp
 * @brief Intelligent file renaming implementation
 */

#include <fileforge/renamer.hpp>
#include <fileforge/logger.hpp>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <regex>
#include <sstream>

namespace fileforge {

Renamer::Renamer(const Config& config)
    : m_config(config) {}

void Renamer::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_usedNames.clear();
    m_versionCounters.clear();
}

std::string Renamer::sanitizeFilename(const std::string& filename) {
    std::string result;
    result.reserve(filename.size());

    for (char c : filename) {
        // Replace invalid characters
        switch (c) {
            case '/':
            case '\\':
            case ':':
            case '*':
            case '?':
            case '"':
            case '<':
            case '>':
            case '|':
                result += '_';
                break;
            default:
                if (std::isprint(static_cast<unsigned char>(c))) {
                    result += c;
                }
                break;
        }
    }

    // Trim leading/trailing spaces and dots
    auto start = result.find_first_not_of(" .");
    auto end = result.find_last_not_of(" .");

    if (start == std::string::npos) {
        return "file";
    }

    return result.substr(start, end - start + 1);
}

std::string Renamer::extractDescriptor(const std::string& filename) {
    // Remove extension
    auto dotPos = filename.rfind('.');
    std::string baseName = (dotPos != std::string::npos)
        ? filename.substr(0, dotPos)
        : filename;

    // Remove common prefixes like IMG_, DSC_, VID_, etc.
    static const std::vector<std::string> prefixes = {
        "IMG_", "DSC_", "VID_", "MOV_", "MVI_", "DCIM_",
        "Screenshot_", "Screen Shot ", "Photo_", "Video_",
        "Document_", "Scan_", "Copy of ", "Copy_"
    };

    for (const auto& prefix : prefixes) {
        if (baseName.size() > prefix.size() &&
            baseName.compare(0, prefix.size(), prefix) == 0) {
            baseName = baseName.substr(prefix.size());
            break;
        }
    }

    // Remove numeric-only names or timestamps
    static const std::regex numericPattern(R"(^\d+$)");
    static const std::regex timestampPattern(R"(^\d{4}[-_]?\d{2}[-_]?\d{2}[-_T]?\d{2}[-_:]?\d{2}[-_:]?\d{2}.*$)");

    if (std::regex_match(baseName, numericPattern) ||
        std::regex_match(baseName, timestampPattern)) {
        return "file";
    }

    // Clean up the descriptor
    std::string descriptor = sanitizeFilename(baseName);

    // Replace multiple underscores/spaces with single underscore
    static const std::regex multiSeparator(R"([_\s-]+)");
    descriptor = std::regex_replace(descriptor, multiSeparator, "_");

    // Limit length
    if (descriptor.size() > 30) {
        descriptor = descriptor.substr(0, 30);
        // Don't end with underscore
        while (!descriptor.empty() && descriptor.back() == '_') {
            descriptor.pop_back();
        }
    }

    // Convert to lowercase
    std::transform(descriptor.begin(), descriptor.end(), descriptor.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return descriptor.empty() ? "file" : descriptor;
}

std::string Renamer::generateDateString(const FileInfo& fileInfo) const {
    try {
        // Convert file_time_type to system_clock
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            fileInfo.lastModified - fs::file_time_type::clock::now() +
            std::chrono::system_clock::now());

        auto time = std::chrono::system_clock::to_time_t(sctp);

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), m_config.renamePattern().dateFormat.c_str());
        return oss.str();
    } catch (...) {
        // Fallback to current date
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), m_config.renamePattern().dateFormat.c_str());
        return oss.str();
    }
}

int Renamer::getNextVersion(const std::string& baseName) {
    // Lock already held by caller
    auto it = m_versionCounters.find(baseName);
    if (it == m_versionCounters.end()) {
        m_versionCounters[baseName] = 1;
        return 1;
    }
    return ++it->second;
}

std::string Renamer::buildFilename(const std::string& category,
                                   const std::string& date,
                                   const std::string& descriptor,
                                   int version,
                                   const std::string& extension) const {
    const auto& pattern = m_config.renamePattern();
    std::ostringstream oss;

    bool first = true;

    if (pattern.includeCategory && !category.empty()) {
        oss << category;
        first = false;
    }

    if (pattern.includeDate && !date.empty()) {
        if (!first) oss << pattern.separator;
        oss << date;
        first = false;
    }

    if (pattern.includeDescriptor && !descriptor.empty()) {
        if (!first) oss << pattern.separator;
        oss << descriptor;
        first = false;
    }

    if (pattern.includeVersion) {
        if (!first) oss << pattern.separator;
        oss << "v" << version;
    }

    // Add extension
    if (!extension.empty()) {
        if (extension[0] != '.') {
            oss << '.';
        }
        oss << extension;
    }

    return oss.str();
}

std::string Renamer::generateName(const FileInfo& fileInfo,
                                  const fs::path& destinationDir) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Get components
    std::string categoryStr = categoryToString(fileInfo.category);
    std::transform(categoryStr.begin(), categoryStr.end(), categoryStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::string dateStr = generateDateString(fileInfo);
    std::string descriptor = extractDescriptor(fileInfo.name);

    // Build base name for version tracking
    std::string baseName = categoryStr + "_" + dateStr + "_" + descriptor;

    // Generate filename with version
    int version = 1;
    std::string newName;

    do {
        version = getNextVersion(baseName);
        newName = buildFilename(categoryStr, dateStr, descriptor,
                               version, fileInfo.extension);

        // Check for collision in used names
        if (m_usedNames.find(newName) != m_usedNames.end()) {
            continue;
        }

        // Check for collision in destination directory
        if (wouldCollide(newName, destinationDir)) {
            continue;
        }

        break;
    } while (version < 10000); // Safety limit

    m_usedNames.insert(newName);
    return newName;
}

std::map<fs::path, std::string> Renamer::generateNames(
    const std::vector<FileInfo>& files,
    const fs::path& destinationDir) {

    std::map<fs::path, std::string> nameMap;

    for (const auto& file : files) {
        std::string newName = generateName(file, destinationDir);
        nameMap[file.path] = newName;
    }

    return nameMap;
}

bool Renamer::wouldCollide(const std::string& name,
                           const fs::path& directory) const {
    if (directory.empty()) {
        return false;
    }

    std::error_code ec;
    return fs::exists(directory / name, ec);
}

} // namespace fileforge
