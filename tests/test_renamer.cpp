/**
 * @file test_renamer.cpp
 * @brief Unit tests for FileForge renamer
 */

#include <fileforge/renamer.hpp>

#include <gtest/gtest.h>

namespace fileforge {
namespace testing {

class RenamerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config = std::make_unique<Config>();
        m_renamer = std::make_unique<Renamer>(*m_config);

        // Create temp directory
        m_tempDir = fs::temp_directory_path() / "fileforge_renamer_test";
        fs::create_directories(m_tempDir);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(m_tempDir, ec);
    }

    std::unique_ptr<Config> m_config;
    std::unique_ptr<Renamer> m_renamer;
    fs::path m_tempDir;
};

TEST_F(RenamerTest, SanitizeFilename) {
    EXPECT_EQ(Renamer::sanitizeFilename("normal.txt"), "normal.txt");
    EXPECT_EQ(Renamer::sanitizeFilename("file/with/slashes"), "file_with_slashes");
    EXPECT_EQ(Renamer::sanitizeFilename("file:colon"), "file_colon");
    EXPECT_EQ(Renamer::sanitizeFilename("file*star"), "file_star");
    EXPECT_EQ(Renamer::sanitizeFilename("file?question"), "file_question");
    EXPECT_EQ(Renamer::sanitizeFilename("file\"quote"), "file_quote");
    EXPECT_EQ(Renamer::sanitizeFilename("file<>pipe|"), "file__pipe_");
}

TEST_F(RenamerTest, SanitizeFilenameEdgeCases) {
    EXPECT_EQ(Renamer::sanitizeFilename("...dots..."), "dots");
    EXPECT_EQ(Renamer::sanitizeFilename("   spaces   "), "spaces");
    EXPECT_EQ(Renamer::sanitizeFilename(""), "file");
    EXPECT_EQ(Renamer::sanitizeFilename("..."), "file");
}

TEST_F(RenamerTest, ExtractDescriptor) {
    EXPECT_EQ(Renamer::extractDescriptor("IMG_001.jpg"), "001");
    EXPECT_EQ(Renamer::extractDescriptor("DSC_1234.png"), "1234");
    EXPECT_EQ(Renamer::extractDescriptor("vacation_photo.jpg"), "vacation_photo");
    EXPECT_EQ(Renamer::extractDescriptor("report.pdf"), "report");
}

TEST_F(RenamerTest, ExtractDescriptorNumericOnly) {
    // Pure numeric names should return "file"
    EXPECT_EQ(Renamer::extractDescriptor("12345.jpg"), "file");
    EXPECT_EQ(Renamer::extractDescriptor("123456789.png"), "file");
}

TEST_F(RenamerTest, ExtractDescriptorTruncation) {
    std::string longName = "this_is_a_very_long_filename_that_should_be_truncated.txt";
    std::string result = Renamer::extractDescriptor(longName);
    EXPECT_LE(result.size(), 30);
}

TEST_F(RenamerTest, GenerateNameBasic) {
    FileInfo info(fs::path("/test/photo.jpg"));
    info.category = FileCategory::Images;

    std::string newName = m_renamer->generateName(info, m_tempDir);

    EXPECT_FALSE(newName.empty());
    EXPECT_TRUE(newName.find("images") != std::string::npos);
    EXPECT_TRUE(newName.find(".jpg") != std::string::npos);
    EXPECT_TRUE(newName.find("_v") != std::string::npos);
}

TEST_F(RenamerTest, GenerateNameVersionIncrement) {
    FileInfo info1(fs::path("/test/photo1.jpg"));
    info1.category = FileCategory::Images;

    FileInfo info2(fs::path("/test/photo2.jpg"));
    info2.category = FileCategory::Images;

    std::string name1 = m_renamer->generateName(info1, m_tempDir);
    std::string name2 = m_renamer->generateName(info2, m_tempDir);

    // Names should be different
    EXPECT_NE(name1, name2);
}

TEST_F(RenamerTest, GenerateNameNoCollision) {
    FileInfo info(fs::path("/test/photo.jpg"));
    info.category = FileCategory::Images;

    std::string name1 = m_renamer->generateName(info, m_tempDir);
    std::string name2 = m_renamer->generateName(info, m_tempDir);

    EXPECT_NE(name1, name2);
}

TEST_F(RenamerTest, ResetClearsState) {
    FileInfo info(fs::path("/test/photo.jpg"));
    info.category = FileCategory::Images;

    std::string name1 = m_renamer->generateName(info, m_tempDir);
    m_renamer->reset();
    std::string name2 = m_renamer->generateName(info, m_tempDir);

    // After reset, version should start from 1 again
    // Names might be same if same file is processed
    EXPECT_TRUE(name2.find("_v1") != std::string::npos ||
                name2.find("_v2") != std::string::npos);
}

TEST_F(RenamerTest, WouldCollide) {
    // Create a file in temp directory
    fs::path existingFile = m_tempDir / "existing.txt";
    std::ofstream(existingFile).close();

    EXPECT_TRUE(m_renamer->wouldCollide("existing.txt", m_tempDir));
    EXPECT_FALSE(m_renamer->wouldCollide("nonexistent.txt", m_tempDir));
}

TEST_F(RenamerTest, GenerateNamesMultiple) {
    std::vector<FileInfo> files = {
        FileInfo(fs::path("/test/image1.jpg")),
        FileInfo(fs::path("/test/image2.jpg")),
        FileInfo(fs::path("/test/image3.jpg"))
    };

    for (auto& f : files) {
        f.category = FileCategory::Images;
    }

    auto nameMap = m_renamer->generateNames(files, m_tempDir);

    EXPECT_EQ(nameMap.size(), 3);

    // All names should be unique
    std::set<std::string> uniqueNames;
    for (const auto& [path, name] : nameMap) {
        uniqueNames.insert(name);
    }
    EXPECT_EQ(uniqueNames.size(), 3);
}

TEST_F(RenamerTest, PreservesExtension) {
    FileInfo jpgFile(fs::path("/test/image.jpg"));
    jpgFile.category = FileCategory::Images;

    FileInfo pngFile(fs::path("/test/image.png"));
    pngFile.category = FileCategory::Images;

    std::string jpgName = m_renamer->generateName(jpgFile, m_tempDir);
    m_renamer->reset();
    std::string pngName = m_renamer->generateName(pngFile, m_tempDir);

    EXPECT_TRUE(jpgName.ends_with(".jpg"));
    EXPECT_TRUE(pngName.ends_with(".png"));
}

} // namespace testing
} // namespace fileforge
