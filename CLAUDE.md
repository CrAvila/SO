# CLAUDE.md - AI Assistant Guidelines for FileForge

## Project Overview

**Project Name:** FileForge - Forging Order from Chaos
**Tagline:** "Transform your cluttered folders into perfectly organized collections"
**Repository:** SO
**Owner:** CrAvila
**License:** Open Source (MIT)
**Platform:** Cross-platform (Windows, Linux, macOS)
**Language:** C++20

FileForge is a high-performance, cross-platform file organization utility that automatically categorizes, organizes, and intelligently renames files. Originally conceived as an Operating Systems final project at UPAEP (Universidad Popular Autónoma del Estado de Puebla), this tool leverages parallelization and modern C++20 best practices to forge order from digital chaos.

## Key Features

- **Automatic File Organization**: Sorts files by type into organized folders
- **Intelligent Renaming**: Renames files based on type, content, date, and metadata
- **Duplicate Handling**: Detects similar files and manages versions
- **Parallelization**: Uses multi-threading (std::thread, std::async) for high-performance
- **Comprehensive Logging**: Full audit trail in JSON format for all operations
- **Cross-Platform**: Native support for Windows, Linux, and macOS
- **Dual Interface**: Both CLI and GUI interfaces available
- **Safe Operations**: Dry-run mode, undo capability, and backup options

## Technology Stack

### Core Application (C++)
- **Language**: C++20
- **Build System**: CMake 3.20+
- **GUI Framework**: Qt6 (cross-platform)
- **CLI Parser**: CLI11
- **JSON Library**: nlohmann/json
- **Testing**: Google Test (gtest)
- **File System**: std::filesystem
- **Parallelization**: std::thread, std::async, std::execution

### Landing Page
- **Framework**: React 18+
- **Build Tool**: Vite
- **Styling**: Tailwind CSS
- **Deployment**: GitHub Pages / Vercel

## Repository Structure

```
SO/
├── CLAUDE.md                 # AI assistant guidelines (this file)
├── README.md                 # Project documentation
├── LICENSE                   # MIT License
├── CMakeLists.txt            # Root CMake configuration
├── .gitignore                # Git ignore rules
│
├── src/                      # Source code
│   ├── core/                 # Core library
│   │   ├── CMakeLists.txt
│   │   ├── scanner.hpp       # File system scanner
│   │   ├── scanner.cpp
│   │   ├── categorizer.hpp   # File type categorization
│   │   ├── categorizer.cpp
│   │   ├── renamer.hpp       # Intelligent renaming
│   │   ├── renamer.cpp
│   │   ├── organizer.hpp     # Main organization engine
│   │   ├── organizer.cpp
│   │   ├── logger.hpp        # Logging system
│   │   ├── logger.cpp
│   │   ├── config.hpp        # Configuration management
│   │   ├── config.cpp
│   │   └── types.hpp         # Common types and structures
│   │
│   ├── cli/                  # Command-line interface
│   │   ├── CMakeLists.txt
│   │   └── main.cpp          # CLI entry point
│   │
│   └── gui/                  # Graphical interface (Qt6)
│       ├── CMakeLists.txt
│       ├── main.cpp          # GUI entry point
│       ├── mainwindow.hpp
│       ├── mainwindow.cpp
│       ├── mainwindow.ui
│       └── resources/        # GUI resources
│
├── include/                  # Public headers
│   └── fileforge/
│       └── fileforge.hpp      # Main include header
│
├── tests/                    # Test suite
│   ├── CMakeLists.txt
│   ├── test_scanner.cpp
│   ├── test_categorizer.cpp
│   ├── test_renamer.cpp
│   ├── test_organizer.cpp
│   └── test_fixtures/        # Test data
│
├── docs/                     # Documentation
│   └── landing/              # React landing page
│       ├── package.json
│       ├── vite.config.ts
│       ├── tailwind.config.js
│       ├── src/
│       │   ├── App.tsx
│       │   ├── main.tsx
│       │   └── components/
│       └── public/
│           └── assets/
│
├── config/                   # Configuration files
│   └── default_config.json   # Default configuration
│
└── scripts/                  # Build and utility scripts
    ├── build.sh              # Linux/macOS build script
    └── build.bat             # Windows build script
```

## Development Setup

### Prerequisites

- **C++ Compiler**: GCC 11+ / Clang 14+ / MSVC 2022+
- **CMake**: 3.20+
- **Qt6**: 6.2+ (for GUI)
- **Node.js**: 18+ (for landing page)
- **Git**: For version control

### Building the Project

#### Linux/macOS
```bash
# Clone the repository
git clone <repository-url>
cd SO

# Create build directory
mkdir build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# Run tests
ctest --output-on-failure

# Install (optional)
sudo cmake --install .
```

#### Windows (MSVC)
```powershell
# Clone and navigate
git clone <repository-url>
cd SO

# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Run tests
ctest -C Release --output-on-failure
```

### Running the Application

```bash
# CLI - Organize current directory
./fileforge-cli .

# CLI - Organize specific directory
./fileforge-cli /path/to/folder

# CLI - Organize Downloads folder
./fileforge-cli ~/Downloads

# CLI - Dry run (preview changes)
./fileforge-cli --dry-run /path/to/folder

# CLI - With custom config
./fileforge-cli --config config.json /path/to/folder

# GUI - Launch graphical interface
./fileforge-gui
```

## Development Workflow

### Branch Naming Convention

- Feature branches: `feature/<description>`
- Bug fixes: `fix/<description>`
- Documentation: `docs/<description>`
- AI-assisted branches: `claude/<description>`

### Commit Guidelines

1. Write clear, descriptive commit messages
2. Use imperative mood ("Add feature" not "Added feature")
3. Keep commits focused and atomic
4. Reference issues when applicable

Example:
```
Add parallel file scanning with thread pool

- Implement std::async for concurrent directory traversal
- Add --threads CLI option for controlling parallelism
- Update logger to be thread-safe with mutex
```

### Testing

```bash
# Run all tests
cd build && ctest --output-on-failure

# Run specific test
./tests/test_scanner

# Run with verbose output
ctest -V

# Run with memory check (Linux)
ctest -T memcheck
```

## Code Conventions

### C++ Style Guide

- **Standard**: C++20
- **Naming**:
  - Classes: `PascalCase` (e.g., `FileScanner`)
  - Functions: `camelCase` (e.g., `scanDirectory`)
  - Variables: `camelCase` (e.g., `fileCount`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_THREADS`)
  - Member variables: `m_` prefix (e.g., `m_config`)
  - Namespaces: `lowercase` (e.g., `fileforge`)
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style (opening brace on same line)
- **Headers**: Use `#pragma once` for include guards
- **Includes**: Group and sort (standard library, external, project)

### Example Code Style

```cpp
#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "types.hpp"

namespace fileforge {

class FileScanner {
public:
    explicit FileScanner(const Config& config);
    ~FileScanner() = default;

    // Scan directory for files
    std::vector<FileInfo> scanDirectory(const std::filesystem::path& path);

    // Scan with parallelization
    std::vector<FileInfo> scanParallel(const std::filesystem::path& path,
                                        size_t numThreads = 4);

private:
    Config m_config;
    std::mutex m_mutex;

    void processFile(const std::filesystem::path& filePath,
                     std::vector<FileInfo>& results);
};

} // namespace fileforge
```

### File Organization Categories

Default categories (customizable via config):

| Category       | Extensions                                    |
|----------------|-----------------------------------------------|
| Images         | .jpg, .jpeg, .png, .gif, .svg, .webp, .bmp    |
| Documents      | .pdf, .doc, .docx, .txt, .rtf, .odt, .md      |
| Spreadsheets   | .xls, .xlsx, .csv, .ods                       |
| Presentations  | .ppt, .pptx, .odp, .key                       |
| Videos         | .mp4, .mkv, .avi, .mov, .wmv, .webm           |
| Audio          | .mp3, .wav, .flac, .aac, .ogg, .m4a           |
| Archives       | .zip, .rar, .7z, .tar, .gz, .bz2              |
| Code           | .cpp, .hpp, .c, .h, .py, .js, .ts, .java, .rs |
| Data           | .json, .xml, .yaml, .sql, .db, .sqlite        |
| Executables    | .exe, .msi, .dmg, .app, .deb, .rpm, .appimage |
| Other          | Uncategorized files                           |

### Intelligent Renaming Format

Format: `{category}_{date}_{descriptor}_{version}.{ext}`

Examples:
- `image_2026-01-22_photo_v1.jpg`
- `document_2026-01-15_report_v2.pdf`
- `video_2025-12-01_recording_v1.mp4`

## Key Commands

```bash
# Build commands
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Run tests
cd build && ctest --output-on-failure

# Format code (if clang-format is installed)
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Static analysis (if clang-tidy is installed)
clang-tidy src/**/*.cpp -- -std=c++20

# Landing page development
cd docs/landing && npm install && npm run dev

# Landing page build
cd docs/landing && npm run build
```

## Guidelines for AI Assistants

### Architecture Principles

1. **Safety First**: Never delete files without explicit user confirmation
2. **Reversibility**: All operations should be undoable via operation logs
3. **Performance**: Use parallelization for I/O-bound operations
4. **Cross-Platform**: Use `std::filesystem` for all path operations
5. **RAII**: Use smart pointers and RAII patterns
6. **Thread Safety**: Protect shared resources with mutexes

### Before Making Changes

1. Read and understand the existing code structure
2. Follow established patterns in the codebase
3. Ensure changes compile on all target platforms
4. Run tests before and after changes

### When Writing Code

1. Use `std::filesystem::path` for all file paths
2. Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
3. Prefer `std::string_view` for read-only string parameters
4. Use `constexpr` and `const` wherever possible
5. Add unit tests for new functionality
6. Document public APIs with Doxygen-style comments

### Security Considerations

1. Validate all user inputs and file paths
2. Prevent path traversal attacks (check for `..`)
3. Sanitize file names before operations
4. Use secure file operations (check permissions)
5. Never execute code from file contents

### Performance Guidelines

1. Use `std::async` or thread pools for parallel operations
2. Process files in batches for memory efficiency
3. Use move semantics to avoid unnecessary copies
4. Cache file metadata to reduce I/O
5. Use `reserve()` for vectors when size is known

### Error Handling

1. Use exceptions for exceptional conditions
2. Return `std::optional` or `std::expected` for expected failures
3. Log errors with context information
4. Provide user-friendly error messages
5. Never silently ignore errors

## Logging Format

All operations logged in JSON Lines format:

```json
{"timestamp":"2026-01-22T10:30:00Z","level":"INFO","action":"scan","path":"/home/user/Downloads","files_found":150}
{"timestamp":"2026-01-22T10:30:01Z","level":"INFO","action":"move","source":"/path/from/file.jpg","destination":"/path/to/Images/file.jpg","status":"success"}
{"timestamp":"2026-01-22T10:30:01Z","level":"INFO","action":"rename","old_name":"IMG_001.jpg","new_name":"image_2026-01-22_photo_v1.jpg","status":"success"}
```

## Configuration File Format

```json
{
  "version": "1.0",
  "categories": {
    "Images": [".jpg", ".jpeg", ".png", ".gif", ".svg", ".webp", ".bmp"],
    "Documents": [".pdf", ".doc", ".docx", ".txt", ".rtf", ".odt", ".md"],
    "Videos": [".mp4", ".mkv", ".avi", ".mov", ".wmv", ".webm"],
    "Audio": [".mp3", ".wav", ".flac", ".aac", ".ogg", ".m4a"],
    "Archives": [".zip", ".rar", ".7z", ".tar", ".gz"],
    "Code": [".cpp", ".hpp", ".c", ".h", ".py", ".js", ".ts"],
    "Other": []
  },
  "options": {
    "create_organized_folder": true,
    "organized_folder_name": "Organized",
    "enable_renaming": true,
    "rename_format": "{category}_{date}_{descriptor}_{version}",
    "dry_run": false,
    "parallel_threads": 4,
    "generate_log": true,
    "log_file": "fileforge_log.jsonl"
  }
}
```

## Roadmap

### Phase 1 - Core (In Progress)
- [ ] Core file organization engine
- [ ] File type categorization
- [ ] Intelligent renaming system
- [ ] Parallelization support
- [ ] Comprehensive logging
- [ ] CLI interface

### Phase 2 - GUI & Polish
- [ ] Cross-platform Qt6 GUI
- [ ] Landing page with React
- [ ] Documentation and README

### Phase 3 - Advanced Features
- [ ] File content analysis (for intelligent naming)
- [ ] Duplicate file detection
- [ ] Cloud storage integration
- [ ] Plugin system for custom categorizers
- [ ] Scheduled automatic organization
- [ ] Mobile companion app

---

*Last updated: January 2026*
*This document should be updated as the project evolves.*
