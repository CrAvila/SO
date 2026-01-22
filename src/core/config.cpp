/**
 * @file config.cpp
 * @brief Configuration management implementation
 */

#include <fileforge/config.hpp>

#include <fstream>
#include <nlohmann/json.hpp>

namespace fileforge {

using json = nlohmann::json;

Config::Config() {
    loadDefaults();
}

void Config::loadDefaults() {
    m_dryRun = false;
    m_createOrganizedFolder = true;
    m_organizedFolderName = "Organized";
    m_enableRenaming = true;
    m_parallelThreads = 4;
    m_generateLog = true;
    m_logFile = "fileforge_log.jsonl";
    m_recursive = false;
    m_includeHidden = false;
    m_preserveStructure = false;

    m_renamePattern = RenamePattern{
        .includeCategory = true,
        .includeDate = true,
        .includeDescriptor = true,
        .includeVersion = true,
        .separator = "_",
        .dateFormat = "%Y-%m-%d"
    };

    initializeDefaultExtensions();
}

void Config::initializeDefaultExtensions() {
    m_categoryExtensions.clear();

    m_categoryExtensions[FileCategory::Images] = {
        ".jpg", ".jpeg", ".png", ".gif", ".svg", ".webp", ".bmp",
        ".ico", ".tiff", ".tif", ".heic", ".heif", ".raw", ".psd"
    };

    m_categoryExtensions[FileCategory::Documents] = {
        ".pdf", ".doc", ".docx", ".txt", ".rtf", ".odt", ".md",
        ".tex", ".pages", ".epub", ".mobi"
    };

    m_categoryExtensions[FileCategory::Spreadsheets] = {
        ".xls", ".xlsx", ".csv", ".ods", ".numbers", ".tsv"
    };

    m_categoryExtensions[FileCategory::Presentations] = {
        ".ppt", ".pptx", ".odp", ".key"
    };

    m_categoryExtensions[FileCategory::Videos] = {
        ".mp4", ".mkv", ".avi", ".mov", ".wmv", ".webm", ".flv",
        ".m4v", ".mpeg", ".mpg", ".3gp"
    };

    m_categoryExtensions[FileCategory::Audio] = {
        ".mp3", ".wav", ".flac", ".aac", ".ogg", ".m4a", ".wma",
        ".aiff", ".opus", ".mid", ".midi"
    };

    m_categoryExtensions[FileCategory::Archives] = {
        ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz",
        ".tgz", ".tbz2", ".lz", ".lzma"
    };

    m_categoryExtensions[FileCategory::Code] = {
        ".cpp", ".hpp", ".c", ".h", ".py", ".js", ".ts", ".java",
        ".rs", ".go", ".rb", ".php", ".swift", ".kt", ".scala",
        ".cs", ".vb", ".lua", ".pl", ".sh", ".bash", ".zsh",
        ".ps1", ".bat", ".cmd"
    };

    m_categoryExtensions[FileCategory::Data] = {
        ".json", ".xml", ".yaml", ".yml", ".sql", ".db", ".sqlite",
        ".sqlite3", ".mdb", ".accdb", ".toml", ".ini", ".cfg"
    };

    m_categoryExtensions[FileCategory::Executables] = {
        ".exe", ".msi", ".dmg", ".app", ".deb", ".rpm", ".appimage",
        ".apk", ".ipa", ".jar", ".run"
    };

    m_categoryExtensions[FileCategory::Other] = {};
}

bool Config::loadFromFile(const fs::path& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        // Load options
        if (j.contains("options")) {
            const auto& opts = j["options"];

            if (opts.contains("dry_run"))
                m_dryRun = opts["dry_run"].get<bool>();
            if (opts.contains("create_organized_folder"))
                m_createOrganizedFolder = opts["create_organized_folder"].get<bool>();
            if (opts.contains("organized_folder_name"))
                m_organizedFolderName = opts["organized_folder_name"].get<std::string>();
            if (opts.contains("enable_renaming"))
                m_enableRenaming = opts["enable_renaming"].get<bool>();
            if (opts.contains("parallel_threads"))
                m_parallelThreads = opts["parallel_threads"].get<std::size_t>();
            if (opts.contains("generate_log"))
                m_generateLog = opts["generate_log"].get<bool>();
            if (opts.contains("log_file"))
                m_logFile = opts["log_file"].get<std::string>();
            if (opts.contains("recursive"))
                m_recursive = opts["recursive"].get<bool>();
            if (opts.contains("include_hidden"))
                m_includeHidden = opts["include_hidden"].get<bool>();
        }

        // Load rename pattern
        if (j.contains("rename_pattern")) {
            const auto& rp = j["rename_pattern"];

            if (rp.contains("include_category"))
                m_renamePattern.includeCategory = rp["include_category"].get<bool>();
            if (rp.contains("include_date"))
                m_renamePattern.includeDate = rp["include_date"].get<bool>();
            if (rp.contains("include_descriptor"))
                m_renamePattern.includeDescriptor = rp["include_descriptor"].get<bool>();
            if (rp.contains("include_version"))
                m_renamePattern.includeVersion = rp["include_version"].get<bool>();
            if (rp.contains("separator"))
                m_renamePattern.separator = rp["separator"].get<std::string>();
            if (rp.contains("date_format"))
                m_renamePattern.dateFormat = rp["date_format"].get<std::string>();
        }

        // Load categories
        if (j.contains("categories")) {
            const auto& cats = j["categories"];

            for (const auto& [key, value] : cats.items()) {
                auto category = stringToCategory(key);
                if (category.has_value() && value.is_array()) {
                    std::vector<std::string> extensions;
                    for (const auto& ext : value) {
                        extensions.push_back(ext.get<std::string>());
                    }
                    m_categoryExtensions[category.value()] = std::move(extensions);
                }
            }
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToFile(const fs::path& path) const {
    try {
        json j;

        // Version
        j["version"] = "1.0";

        // Options
        j["options"] = {
            {"dry_run", m_dryRun},
            {"create_organized_folder", m_createOrganizedFolder},
            {"organized_folder_name", m_organizedFolderName},
            {"enable_renaming", m_enableRenaming},
            {"parallel_threads", m_parallelThreads},
            {"generate_log", m_generateLog},
            {"log_file", m_logFile},
            {"recursive", m_recursive},
            {"include_hidden", m_includeHidden}
        };

        // Rename pattern
        j["rename_pattern"] = {
            {"include_category", m_renamePattern.includeCategory},
            {"include_date", m_renamePattern.includeDate},
            {"include_descriptor", m_renamePattern.includeDescriptor},
            {"include_version", m_renamePattern.includeVersion},
            {"separator", m_renamePattern.separator},
            {"date_format", m_renamePattern.dateFormat}
        };

        // Categories
        json cats;
        for (const auto& [category, extensions] : m_categoryExtensions) {
            cats[categoryToString(category)] = extensions;
        }
        j["categories"] = cats;

        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }

        file << j.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

const std::vector<std::string>& Config::getExtensions(FileCategory category) const {
    static const std::vector<std::string> empty;
    auto it = m_categoryExtensions.find(category);
    if (it != m_categoryExtensions.end()) {
        return it->second;
    }
    return empty;
}

void Config::setExtensions(FileCategory category, const std::vector<std::string>& extensions) {
    m_categoryExtensions[category] = extensions;
}

void Config::addExtension(FileCategory category, const std::string& extension) {
    m_categoryExtensions[category].push_back(extension);
}

} // namespace fileforge
