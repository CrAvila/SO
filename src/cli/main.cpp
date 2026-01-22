/**
 * @file main.cpp
 * @brief FileForge CLI Application
 *
 * FileForge - Forging Order from Chaos
 * A high-performance file organization utility
 */

#include <fileforge/fileforge.hpp>

#include <CLI/CLI.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

// ANSI color codes
constexpr const char* RESET = "\033[0m";
constexpr const char* BOLD = "\033[1m";
constexpr const char* RED = "\033[31m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* BLUE = "\033[34m";
constexpr const char* CYAN = "\033[36m";

bool useColors = true;

std::string colorize(const std::string& text, const char* color) {
    if (!useColors) return text;
    return std::string(color) + text + RESET;
}

void printBanner() {
    std::cout << colorize(R"(
  _____ _ _      _____
 |  ___(_) | ___|  ___|__  _ __ __ _  ___
 | |_  | | |/ _ \ |_ / _ \| '__/ _` |/ _ \
 |  _| | | |  __/  _| (_) | | | (_| |  __/
 |_|   |_|_|\___|_|  \___/|_|  \__, |\___|
                               |___/
)", CYAN) << "\n";
    std::cout << colorize("  Forging Order from Chaos", BOLD) << "\n";
    std::cout << "  Version " << fileforge::version() << "\n\n";
}

void printProgress(std::size_t current, std::size_t total, const std::string& filename) {
    int percentage = static_cast<int>((current * 100) / std::max(total, std::size_t{1}));
    int barWidth = 40;
    int filled = (percentage * barWidth) / 100;

    std::cout << "\r" << colorize("[", CYAN);

    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) {
            std::cout << colorize("=", GREEN);
        } else if (i == filled) {
            std::cout << colorize(">", YELLOW);
        } else {
            std::cout << " ";
        }
    }

    std::cout << colorize("]", CYAN) << " "
              << std::setw(3) << percentage << "% "
              << "(" << current << "/" << total << ") ";

    // Truncate filename if too long
    std::string displayName = filename;
    if (displayName.size() > 30) {
        displayName = "..." + displayName.substr(displayName.size() - 27);
    }
    std::cout << displayName;

    // Clear rest of line
    std::cout << "\033[K" << std::flush;
}

void printStats(const fileforge::OrganizationStats& stats) {
    std::cout << "\n\n" << colorize("Organization Complete!", GREEN) << "\n";
    std::cout << std::string(50, '-') << "\n";

    std::cout << colorize("Total files:     ", BOLD) << stats.totalFiles << "\n";
    std::cout << colorize("Processed:       ", BOLD)
              << colorize(std::to_string(stats.filesProcessed), GREEN) << "\n";
    std::cout << colorize("Moved:           ", BOLD) << stats.filesMoved << "\n";
    std::cout << colorize("Renamed:         ", BOLD) << stats.filesRenamed << "\n";

    if (stats.filesSkipped > 0) {
        std::cout << colorize("Skipped:         ", BOLD)
                  << colorize(std::to_string(stats.filesSkipped), YELLOW) << "\n";
    }

    if (stats.filesFailed > 0) {
        std::cout << colorize("Failed:          ", BOLD)
                  << colorize(std::to_string(stats.filesFailed), RED) << "\n";
    }

    // Format bytes
    double bytes = static_cast<double>(stats.totalBytes);
    std::string unit = "B";
    if (bytes >= 1024 * 1024 * 1024) {
        bytes /= 1024 * 1024 * 1024;
        unit = "GB";
    } else if (bytes >= 1024 * 1024) {
        bytes /= 1024 * 1024;
        unit = "MB";
    } else if (bytes >= 1024) {
        bytes /= 1024;
        unit = "KB";
    }

    std::cout << colorize("Total size:      ", BOLD)
              << std::fixed << std::setprecision(2) << bytes << " " << unit << "\n";

    std::cout << colorize("Duration:        ", BOLD)
              << stats.duration.count() << " ms\n";

    std::cout << colorize("Success rate:    ", BOLD)
              << std::fixed << std::setprecision(1) << stats.successRate() << "%\n";

    std::cout << std::string(50, '-') << "\n";
}

void printCategoryStats(const std::map<fileforge::FileCategory, std::size_t>& stats) {
    std::cout << "\n" << colorize("Files by Category:", BOLD) << "\n";
    std::cout << std::string(30, '-') << "\n";

    for (const auto& [category, count] : stats) {
        if (count > 0) {
            std::cout << std::setw(15) << std::left
                      << fileforge::categoryToString(category) << ": "
                      << count << "\n";
        }
    }
}

} // anonymous namespace

int main(int argc, char** argv) {
    CLI::App app{"FileForge - Forging Order from Chaos"};
    app.set_version_flag("-v,--version", std::string(fileforge::version()));

    // Positional argument: directory
    std::string directory = ".";
    app.add_option("directory", directory, "Directory to organize")
        ->default_str(".")
        ->check(CLI::ExistingDirectory);

    // Options
    bool dryRun = false;
    app.add_flag("-n,--dry-run", dryRun, "Preview changes without modifying files");

    bool recursive = false;
    app.add_flag("-r,--recursive", recursive, "Process subdirectories recursively");

    bool noRename = false;
    app.add_flag("--no-rename", noRename, "Don't rename files, only organize");

    bool includeHidden = false;
    app.add_flag("--hidden", includeHidden, "Include hidden files");

    bool noLog = false;
    app.add_flag("--no-log", noLog, "Don't create a log file");

    std::string configFile;
    app.add_option("-c,--config", configFile, "Configuration file path")
        ->check(CLI::ExistingFile);

    std::string outputDir;
    app.add_option("-o,--output", outputDir, "Output directory (default: <directory>/Organized)");

    std::size_t threads = 4;
    app.add_option("-j,--threads", threads, "Number of parallel threads")
        ->default_val(4)
        ->check(CLI::Range(1, 32));

    bool noColor = false;
    app.add_flag("--no-color", noColor, "Disable colored output");

    bool quiet = false;
    app.add_flag("-q,--quiet", quiet, "Minimal output");

    // Parse arguments
    CLI11_PARSE(app, argc, argv);

    useColors = !noColor;

    if (!quiet) {
        printBanner();
    }

    // Setup configuration
    fileforge::Config config;

    if (!configFile.empty()) {
        if (!config.loadFromFile(configFile)) {
            std::cerr << colorize("Error: Failed to load config file", RED) << "\n";
            return 1;
        }
        if (!quiet) {
            std::cout << "Loaded config: " << configFile << "\n";
        }
    }

    // Apply command-line overrides
    config.setDryRun(dryRun);
    config.setRecursive(recursive);
    config.setEnableRenaming(!noRename);
    config.setIncludeHidden(includeHidden);
    config.setGenerateLog(!noLog);
    config.setParallelThreads(threads);

    if (!outputDir.empty()) {
        config.setCreateOrganizedFolder(false);
    }

    // Initialize logger
    if (config.generateLog()) {
        fileforge::fs::path logPath = directory;
        logPath /= config.logFile();
        fileforge::Logger::instance().initialize(logPath, false);
    }

    // Show configuration
    if (!quiet) {
        std::cout << colorize("Configuration:", BOLD) << "\n";
        std::cout << "  Source:      " << directory << "\n";
        std::cout << "  Destination: " << (outputDir.empty()
            ? (directory + "/" + config.organizedFolderName())
            : outputDir) << "\n";
        std::cout << "  Recursive:   " << (recursive ? "Yes" : "No") << "\n";
        std::cout << "  Renaming:    " << (!noRename ? "Yes" : "No") << "\n";
        std::cout << "  Threads:     " << threads << "\n";
        std::cout << "  Dry run:     " << (dryRun ? colorize("Yes", YELLOW) : "No") << "\n";
        std::cout << "\n";
    }

    // Create organizer
    fileforge::Organizer organizer(config);

    // Set progress callback
    if (!quiet) {
        organizer.setProgressCallback(printProgress);
    }

    // Run organization
    fileforge::OrganizationStats stats;

    if (outputDir.empty()) {
        stats = organizer.organize(directory);
    } else {
        stats = organizer.organize(directory, outputDir);
    }

    // Print results
    if (!quiet) {
        printStats(stats);

        // Print category breakdown
        fileforge::Categorizer categorizer(config);
        auto history = organizer.getOperationHistory();

        std::vector<fileforge::FileInfo> processedFiles;
        for (const auto& record : history) {
            if (record.status == fileforge::OperationStatus::Success ||
                record.status == fileforge::OperationStatus::DryRun) {
                fileforge::FileInfo info;
                info.path = record.sourcePath;
                info.name = record.oldName;
                info.extension = record.sourcePath.extension().string();
                categorizer.categorize(info);
                processedFiles.push_back(info);
            }
        }

        if (!processedFiles.empty()) {
            auto categoryStats = categorizer.getCategoryStats(processedFiles);
            printCategoryStats(categoryStats);
        }
    }

    // Close logger
    fileforge::Logger::instance().close();

    return stats.filesFailed > 0 ? 1 : 0;
}
