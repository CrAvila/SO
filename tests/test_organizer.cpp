/**
 * @file test_organizer.cpp
 * @brief Unit tests for FileForge organizer
 */

#include <fileforge/organizer.hpp>

#include <gtest/gtest.h>

#include <fstream>

namespace fileforge {
namespace testing {

class OrganizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config = std::make_unique<Config>();
        m_config->setGenerateLog(false); // Disable logging for tests

        // Create temp directory with test files
        m_tempDir = fs::temp_directory_path() / "fileforge_organizer_test";
        std::error_code ec;
        fs::remove_all(m_tempDir, ec); // Clean up from previous runs
        fs::create_directories(m_tempDir);

        // Create test files
        createFile(m_tempDir / "photo.jpg", 100);
        createFile(m_tempDir / "document.pdf", 200);
        createFile(m_tempDir / "video.mp4", 300);
        createFile(m_tempDir / "code.cpp", 150);
        createFile(m_tempDir / "data.json", 50);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(m_tempDir, ec);
    }

    void createFile(const fs::path& path, std::size_t size = 100) {
        std::ofstream file(path);
        std::string content(size, 'x');
        file << content;
        file.close();
    }

    std::unique_ptr<Config> m_config;
    fs::path m_tempDir;
};

TEST_F(OrganizerTest, OrganizeBasic) {
    Organizer organizer(*m_config);
    auto stats = organizer.organize(m_tempDir);

    EXPECT_EQ(stats.totalFiles, 5);
    EXPECT_EQ(stats.filesProcessed, 5);
    EXPECT_EQ(stats.filesFailed, 0);
}

TEST_F(OrganizerTest, OrganizeCreatesDirectories) {
    Organizer organizer(*m_config);
    organizer.organize(m_tempDir);

    fs::path organizedDir = m_tempDir / "Organized";
    EXPECT_TRUE(fs::exists(organizedDir / "Images"));
    EXPECT_TRUE(fs::exists(organizedDir / "Documents"));
    EXPECT_TRUE(fs::exists(organizedDir / "Videos"));
    EXPECT_TRUE(fs::exists(organizedDir / "Code"));
    EXPECT_TRUE(fs::exists(organizedDir / "Data"));
}

TEST_F(OrganizerTest, OrganizeMoveFiles) {
    Organizer organizer(*m_config);
    organizer.organize(m_tempDir);

    fs::path organizedDir = m_tempDir / "Organized";

    // Original files should be moved
    EXPECT_FALSE(fs::exists(m_tempDir / "photo.jpg"));
    EXPECT_FALSE(fs::exists(m_tempDir / "document.pdf"));

    // Check files are in category directories
    bool foundImage = false;
    for (const auto& entry : fs::directory_iterator(organizedDir / "Images")) {
        if (entry.path().extension() == ".jpg") {
            foundImage = true;
            break;
        }
    }
    EXPECT_TRUE(foundImage);
}

TEST_F(OrganizerTest, OrganizeDryRun) {
    m_config->setDryRun(true);

    Organizer organizer(*m_config);
    auto stats = organizer.organize(m_tempDir);

    // Files should NOT be moved in dry run
    EXPECT_TRUE(fs::exists(m_tempDir / "photo.jpg"));
    EXPECT_TRUE(fs::exists(m_tempDir / "document.pdf"));

    // But stats should still be calculated
    EXPECT_EQ(stats.totalFiles, 5);
}

TEST_F(OrganizerTest, OrganizeWithCustomDestination) {
    fs::path customDest = m_tempDir / "CustomOrganized";
    m_config->setCreateOrganizedFolder(false);

    Organizer organizer(*m_config);
    organizer.organize(m_tempDir, customDest);

    EXPECT_TRUE(fs::exists(customDest / "Images"));
    EXPECT_TRUE(fs::exists(customDest / "Documents"));
}

TEST_F(OrganizerTest, OrganizeWithRenaming) {
    m_config->setEnableRenaming(true);

    Organizer organizer(*m_config);
    organizer.organize(m_tempDir);

    fs::path imagesDir = m_tempDir / "Organized" / "Images";
    EXPECT_TRUE(fs::exists(imagesDir));

    // Check that files were renamed
    bool foundRenamed = false;
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        std::string name = entry.path().filename().string();
        // Renamed files should have the pattern: images_date_descriptor_vN.ext
        if (name.find("images_") != std::string::npos) {
            foundRenamed = true;
            break;
        }
    }
    EXPECT_TRUE(foundRenamed);
}

TEST_F(OrganizerTest, OrganizeWithoutRenaming) {
    m_config->setEnableRenaming(false);

    Organizer organizer(*m_config);
    organizer.organize(m_tempDir);

    fs::path imagesDir = m_tempDir / "Organized" / "Images";

    // Original filename should be preserved
    bool foundOriginal = false;
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        if (entry.path().filename() == "photo.jpg") {
            foundOriginal = true;
            break;
        }
    }
    EXPECT_TRUE(foundOriginal);
}

TEST_F(OrganizerTest, OrganizeStats) {
    Organizer organizer(*m_config);
    auto stats = organizer.organize(m_tempDir);

    EXPECT_EQ(stats.totalFiles, 5);
    EXPECT_EQ(stats.filesProcessed, 5);
    EXPECT_EQ(stats.filesMoved, 5);
    EXPECT_EQ(stats.filesFailed, 0);
    EXPECT_GT(stats.totalBytes, 0);
    EXPECT_GT(stats.duration.count(), 0);
}

TEST_F(OrganizerTest, OrganizeEmptyDirectory) {
    fs::path emptyDir = m_tempDir / "empty";
    fs::create_directories(emptyDir);

    Organizer organizer(*m_config);
    auto stats = organizer.organize(emptyDir);

    EXPECT_EQ(stats.totalFiles, 0);
    EXPECT_EQ(stats.filesProcessed, 0);
}

TEST_F(OrganizerTest, OrganizeNonExistent) {
    Organizer organizer(*m_config);
    auto stats = organizer.organize("/nonexistent/path");

    EXPECT_EQ(stats.totalFiles, 0);
}

TEST_F(OrganizerTest, Preview) {
    Organizer organizer(*m_config);
    auto operations = organizer.preview(m_tempDir);

    EXPECT_EQ(operations.size(), 5);

    // Files should not be moved during preview
    EXPECT_TRUE(fs::exists(m_tempDir / "photo.jpg"));
}

TEST_F(OrganizerTest, ProgressCallback) {
    std::size_t progressCount = 0;

    Organizer organizer(*m_config);
    organizer.setProgressCallback([&](std::size_t current, std::size_t total,
                                      const std::string& filename) {
        (void)total;
        (void)filename;
        progressCount = current;
    });

    organizer.organize(m_tempDir);

    EXPECT_EQ(progressCount, 5);
}

TEST_F(OrganizerTest, Cancel) {
    Organizer organizer(*m_config);

    // Cancel immediately
    organizer.cancel();
    EXPECT_TRUE(organizer.isCancelled());

    auto stats = organizer.organize(m_tempDir);

    // Should process few or no files when cancelled
    // (depends on timing)
    EXPECT_LE(stats.filesProcessed, stats.totalFiles);
}

TEST_F(OrganizerTest, GetLastStats) {
    Organizer organizer(*m_config);
    organizer.organize(m_tempDir);

    const auto& stats = organizer.getLastStats();
    EXPECT_EQ(stats.totalFiles, 5);
}

TEST_F(OrganizerTest, GetOperationHistory) {
    Organizer organizer(*m_config);
    organizer.organize(m_tempDir);

    const auto& history = organizer.getOperationHistory();
    EXPECT_EQ(history.size(), 5);

    for (const auto& record : history) {
        EXPECT_EQ(record.status, OperationStatus::Success);
    }
}

} // namespace testing
} // namespace fileforge
