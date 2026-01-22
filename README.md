# FileForge

<div align="center">

![FileForge Logo](docs/landing/public/favicon.svg)

**Forging Order from Chaos**

[![License: MIT](https://img.shields.io/badge/License-MIT-orange.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)](https://cmake.org/)

*A high-performance, cross-platform file organization utility*

[Features](#features) | [Installation](#installation) | [Usage](#usage) | [Documentation](#documentation) | [Contributing](#contributing)

</div>

---

## Overview

FileForge is a powerful file organization utility that automatically categorizes, organizes, and intelligently renames files. Originally conceived as an Operating Systems final project at UPAEP (Universidad Popular Autónoma del Estado de Puebla), this tool leverages parallelization and modern C++20 best practices to deliver exceptional performance.

### Key Features

- **Automatic File Organization**: Sorts files by type into 11 intuitive categories
- **Intelligent Renaming**: Renames files based on type, content, date, and metadata
- **Duplicate Handling**: Detects similar files and manages versions automatically
- **Parallelization**: Multi-threaded architecture for processing thousands of files in seconds
- **Comprehensive Logging**: Full audit trail in JSON format for all operations
- **Cross-Platform**: Native support for Windows, Linux, and macOS
- **Dual Interface**: Both CLI and GUI applications available
- **Safe Operations**: Dry-run mode, undo capability, and backup options

## Installation

### Prerequisites

- **C++ Compiler**: GCC 11+ / Clang 14+ / MSVC 2022+
- **CMake**: 3.20+
- **Qt6**: 6.2+ (optional, for GUI)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/CrAvila/SO.git
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

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `FILEFORGE_BUILD_CLI` | ON | Build CLI application |
| `FILEFORGE_BUILD_GUI` | ON | Build GUI application (requires Qt6) |
| `FILEFORGE_BUILD_TESTS` | ON | Build unit tests |

```bash
# Example: Build only CLI without tests
cmake .. -DFILEFORGE_BUILD_GUI=OFF -DFILEFORGE_BUILD_TESTS=OFF
```

## Usage

### Command Line Interface

```bash
# Organize current directory
fileforge-cli .

# Organize specific directory
fileforge-cli /path/to/folder

# Organize Downloads folder
fileforge-cli ~/Downloads

# Dry run (preview changes)
fileforge-cli --dry-run /path/to/folder

# Recursive organization
fileforge-cli --recursive /path/to/folder

# Disable renaming (only organize)
fileforge-cli --no-rename /path/to/folder

# Custom output directory
fileforge-cli -o /path/to/output /path/to/source

# Use custom config file
fileforge-cli --config myconfig.json /path/to/folder
```

### CLI Options

| Option | Short | Description |
|--------|-------|-------------|
| `--dry-run` | `-n` | Preview changes without modifying files |
| `--recursive` | `-r` | Process subdirectories recursively |
| `--no-rename` | | Don't rename files, only organize |
| `--hidden` | | Include hidden files |
| `--no-log` | | Don't create a log file |
| `--config` | `-c` | Configuration file path |
| `--output` | `-o` | Output directory |
| `--threads` | `-j` | Number of parallel threads (default: 4) |
| `--no-color` | | Disable colored output |
| `--quiet` | `-q` | Minimal output |

### Graphical Interface

Launch the GUI application:

```bash
fileforge-gui
```

The GUI provides:
- Drag-and-drop folder selection
- Real-time progress tracking
- Visual category breakdown
- Configuration panel
- Operation log viewer

## File Categories

FileForge organizes files into 11 categories:

| Category | Extensions |
|----------|------------|
| **Images** | .jpg, .jpeg, .png, .gif, .svg, .webp, .bmp, .ico, .tiff, .heic, .raw, .psd |
| **Documents** | .pdf, .doc, .docx, .txt, .rtf, .odt, .md, .tex, .epub, .mobi |
| **Spreadsheets** | .xls, .xlsx, .csv, .ods, .numbers |
| **Presentations** | .ppt, .pptx, .odp, .key |
| **Videos** | .mp4, .mkv, .avi, .mov, .wmv, .webm, .flv, .mpeg |
| **Audio** | .mp3, .wav, .flac, .aac, .ogg, .m4a, .wma, .aiff, .opus |
| **Archives** | .zip, .rar, .7z, .tar, .gz, .bz2, .xz |
| **Code** | .cpp, .hpp, .c, .h, .py, .js, .ts, .java, .rs, .go, .rb, .php, .swift |
| **Data** | .json, .xml, .yaml, .yml, .sql, .db, .sqlite, .toml, .ini |
| **Executables** | .exe, .msi, .dmg, .app, .deb, .rpm, .appimage |
| **Other** | Everything else |

## Intelligent Renaming

Files are renamed using the pattern: `{category}_{date}_{descriptor}_{version}.{ext}`

Examples:
- `photo.jpg` → `images_2026-01-22_photo_v1.jpg`
- `report.pdf` → `documents_2026-01-15_report_v1.pdf`
- `IMG_001.jpg` → `images_2026-01-22_file_v1.jpg`

## Configuration

FileForge can be configured via JSON file:

```json
{
  "version": "1.0",
  "categories": {
    "Images": [".jpg", ".jpeg", ".png", ".gif"],
    "Documents": [".pdf", ".doc", ".docx", ".txt"]
  },
  "options": {
    "create_organized_folder": true,
    "organized_folder_name": "Organized",
    "enable_renaming": true,
    "dry_run": false,
    "parallel_threads": 4,
    "generate_log": true,
    "log_file": "fileforge_log.jsonl"
  }
}
```

## Logging

All operations are logged in JSON Lines format for easy parsing and undo capability:

```json
{"timestamp":"2026-01-22T10:30:00Z","level":"INFO","action":"scan","path":"/home/user/Downloads","files_found":150}
{"timestamp":"2026-01-22T10:30:01Z","level":"INFO","action":"move","source":"/path/from","destination":"/path/to","status":"success"}
```

## Documentation

- [CLAUDE.md](CLAUDE.md) - AI assistant guidelines and architecture documentation
- [API Documentation](docs/) - Generated API documentation
- [Landing Page](docs/landing/) - Project website source

## Project Structure

```
SO/
├── include/fileforge/     # Public headers
├── src/
│   ├── core/              # Core library
│   ├── cli/               # CLI application
│   └── gui/               # Qt6 GUI application
├── tests/                 # Unit tests
├── docs/landing/          # React landing page
├── config/                # Example configurations
└── scripts/               # Build scripts
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Originally developed as an Operating Systems final project at UPAEP
- Built with modern C++20 features
- Uses [nlohmann/json](https://github.com/nlohmann/json) for JSON handling
- Uses [CLI11](https://github.com/CLIUtils/CLI11) for command-line parsing
- GUI powered by [Qt6](https://www.qt.io/)

---

<div align="center">

**FileForge** - *Forging Order from Chaos*

Made with passion by [CrAvila](https://github.com/CrAvila)

</div>
