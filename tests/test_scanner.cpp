/**
 * @file test_scanner.cpp
 * @brief Unit tests for FileForge scanner
 */

#include <fileforge/scanner.hpp>

#include <gtest/gtest.h>

#include <fstream>

namespace fileforge {
namespace testing {

class ScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config = std::make_unique<Config>();
        m_scanner = std::make_unique<Scanner>(*m_config);

        // Create temp directory with test files
        m_tempDir = fs::temp_directory_path() / "fileforge_scanner_test";
        fs::create_directories(m_tempDir);
        fs::create_directories(m_tempDir / "subdir");

        // Create test files
        createFile(m_tempDir / "file1.txt");
        createFile(m_tempDir / "file2.jpg");
        createFile(m_tempDir / "file3.pdf");
        createFile(m_tempDir / ".hidden");
        createFile(m_tempDir / "subdir" / "nested.txt");
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(m_tempDir, ec);
    }

    void createFile(const fs::path& path) {
        std::ofstream file(path);
        file << "test content";
        file.close();
    }

    std::unique_ptr<Config> m_config;
    std::unique_ptr<Scanner> m_scanner;
    fs::path m_tempDir;
};

TEST_F(ScannerTest, ScanNonRecursive) {
    m_config->setRecursive(false);
    m_config->setIncludeHidden(false);

    auto files = m_scanner->scan(m_tempDir);

    // Should find 3 files (file1.txt, file2.jpg, file3.pdf)
    // Not hidden, not in subdir
    EXPECT_EQ(files.size(), 3);
}

TEST_F(ScannerTest, ScanRecursive) {
    m_config->setRecursive(true);
    m_config->setIncludeHidden(false);

    m_scanner = std::make_unique<Scanner>(*m_config);
    auto files = m_scanner->scan(m_tempDir);

    // Should find 4 files (3 in root + 1 in subdir)
    EXPECT_EQ(files.size(), 4);
}

TEST_F(ScannerTest, ScanIncludeHidden) {
    m_config->setRecursive(false);
    m_config->setIncludeHidden(true);

    m_scanner = std::make_unique<Scanner>(*m_config);
    auto files = m_scanner->scan(m_tempDir);

    // Should find 4 files (3 regular + 1 hidden)
    EXPECT_EQ(files.size(), 4);
}

TEST_F(ScannerTest, ScanNonExistent) {
    auto files = m_scanner->scan("/nonexistent/path");
    EXPECT_TRUE(files.empty());
}

TEST_F(ScannerTest, ScanEmptyDirectory) {
    fs::path emptyDir = m_tempDir / "empty";
    fs::create_directories(emptyDir);

    auto files = m_scanner->scan(emptyDir);
    EXPECT_TRUE(files.empty());
}

TEST_F(ScannerTest, FileInfoPopulated) {
    auto files = m_scanner->scan(m_tempDir);
    EXPECT_FALSE(files.empty());

    bool foundTxt = false;
    for (const auto& file : files) {
        if (file.extension == ".txt") {
            foundTxt = true;
            EXPECT_FALSE(file.name.empty());
            EXPECT_FALSE(file.path.empty());
            EXPECT_GT(file.size, 0);
        }
    }
    EXPECT_TRUE(foundTxt);
}

TEST_F(ScannerTest, ScanParallel) {
    m_config->setRecursive(true);
    m_config->setParallelThreads(4);

    m_scanner = std::make_unique<Scanner>(*m_config);
    auto files = m_scanner->scanParallel(m_tempDir);

    EXPECT_FALSE(files.empty());
}

TEST_F(ScannerTest, ScanCancel) {
    m_scanner->cancel();
    auto files = m_scanner->scan(m_tempDir);

    // Should return early when cancelled
    EXPECT_TRUE(m_scanner->isCancelled());
}

TEST_F(ScannerTest, ScanReset) {
    m_scanner->cancel();
    EXPECT_TRUE(m_scanner->isCancelled());

    m_scanner->reset();
    EXPECT_FALSE(m_scanner->isCancelled());
}

TEST_F(ScannerTest, ProgressCallback) {
    std::size_t callbackCount = 0;

    m_scanner->setProgressCallback([&](std::size_t current, std::size_t total) {
        (void)total;
        callbackCount = current;
    });

    auto files = m_scanner->scan(m_tempDir);

    // Callback should have been called at least once
    EXPECT_GT(callbackCount, 0);
}

TEST_F(ScannerTest, GetLastScanCount) {
    auto files = m_scanner->scan(m_tempDir);
    EXPECT_EQ(m_scanner->getLastScanCount(), files.size());
}

TEST_F(ScannerTest, FileExtensionNormalized) {
    createFile(m_tempDir / "UPPERCASE.JPG");

    auto files = m_scanner->scan(m_tempDir);

    bool found = false;
    for (const auto& file : files) {
        if (file.name == "UPPERCASE.JPG") {
            found = true;
            EXPECT_EQ(file.extension, ".jpg"); // Should be lowercase
        }
    }
    EXPECT_TRUE(found);
}

} // namespace testing
} // namespace fileforge
