/**
 * @file test_config.cpp
 * @brief Unit tests for FileForge configuration
 */

#include <fileforge/config.hpp>

#include <gtest/gtest.h>

#include <fstream>

namespace fileforge {
namespace testing {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory for test files
        m_tempDir = fs::temp_directory_path() / "fileforge_test";
        fs::create_directories(m_tempDir);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(m_tempDir, ec);
    }

    fs::path m_tempDir;
};

TEST_F(ConfigTest, DefaultValues) {
    Config config;

    EXPECT_FALSE(config.dryRun());
    EXPECT_TRUE(config.createOrganizedFolder());
    EXPECT_EQ(config.organizedFolderName(), "Organized");
    EXPECT_TRUE(config.enableRenaming());
    EXPECT_EQ(config.parallelThreads(), 4);
    EXPECT_TRUE(config.generateLog());
    EXPECT_FALSE(config.recursive());
    EXPECT_FALSE(config.includeHidden());
}

TEST_F(ConfigTest, Setters) {
    Config config;

    config.setDryRun(true);
    EXPECT_TRUE(config.dryRun());

    config.setRecursive(true);
    EXPECT_TRUE(config.recursive());

    config.setParallelThreads(8);
    EXPECT_EQ(config.parallelThreads(), 8);

    config.setOrganizedFolderName("Custom");
    EXPECT_EQ(config.organizedFolderName(), "Custom");
}

TEST_F(ConfigTest, DefaultExtensions) {
    Config config;

    auto imageExts = config.getExtensions(FileCategory::Images);
    EXPECT_FALSE(imageExts.empty());
    EXPECT_NE(std::find(imageExts.begin(), imageExts.end(), ".jpg"), imageExts.end());
    EXPECT_NE(std::find(imageExts.begin(), imageExts.end(), ".png"), imageExts.end());

    auto docExts = config.getExtensions(FileCategory::Documents);
    EXPECT_FALSE(docExts.empty());
    EXPECT_NE(std::find(docExts.begin(), docExts.end(), ".pdf"), docExts.end());

    auto codeExts = config.getExtensions(FileCategory::Code);
    EXPECT_FALSE(codeExts.empty());
    EXPECT_NE(std::find(codeExts.begin(), codeExts.end(), ".cpp"), codeExts.end());
}

TEST_F(ConfigTest, AddExtension) {
    Config config;

    config.addExtension(FileCategory::Images, ".custom");
    auto exts = config.getExtensions(FileCategory::Images);
    EXPECT_NE(std::find(exts.begin(), exts.end(), ".custom"), exts.end());
}

TEST_F(ConfigTest, SaveAndLoad) {
    Config original;
    original.setDryRun(true);
    original.setRecursive(true);
    original.setParallelThreads(8);
    original.setOrganizedFolderName("TestOrganized");

    fs::path configPath = m_tempDir / "test_config.json";

    // Save
    EXPECT_TRUE(original.saveToFile(configPath));
    EXPECT_TRUE(fs::exists(configPath));

    // Load
    Config loaded;
    EXPECT_TRUE(loaded.loadFromFile(configPath));

    // Verify
    EXPECT_EQ(loaded.dryRun(), original.dryRun());
    EXPECT_EQ(loaded.recursive(), original.recursive());
    EXPECT_EQ(loaded.parallelThreads(), original.parallelThreads());
    EXPECT_EQ(loaded.organizedFolderName(), original.organizedFolderName());
}

TEST_F(ConfigTest, LoadNonExistent) {
    Config config;
    EXPECT_FALSE(config.loadFromFile("/nonexistent/path/config.json"));
}

TEST_F(ConfigTest, LoadInvalidJson) {
    fs::path invalidPath = m_tempDir / "invalid.json";

    std::ofstream file(invalidPath);
    file << "not valid json {{{";
    file.close();

    Config config;
    EXPECT_FALSE(config.loadFromFile(invalidPath));
}

TEST_F(ConfigTest, RenamePattern) {
    Config config;
    const auto& pattern = config.renamePattern();

    EXPECT_TRUE(pattern.includeCategory);
    EXPECT_TRUE(pattern.includeDate);
    EXPECT_TRUE(pattern.includeDescriptor);
    EXPECT_TRUE(pattern.includeVersion);
    EXPECT_EQ(pattern.separator, "_");
}

} // namespace testing
} // namespace fileforge
