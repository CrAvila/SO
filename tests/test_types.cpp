/**
 * @file test_types.cpp
 * @brief Unit tests for FileForge types
 */

#include <fileforge/types.hpp>

#include <gtest/gtest.h>

namespace fileforge {
namespace testing {

TEST(TypesTest, CategoryToString) {
    EXPECT_STREQ(categoryToString(FileCategory::Images), "Images");
    EXPECT_STREQ(categoryToString(FileCategory::Documents), "Documents");
    EXPECT_STREQ(categoryToString(FileCategory::Videos), "Videos");
    EXPECT_STREQ(categoryToString(FileCategory::Audio), "Audio");
    EXPECT_STREQ(categoryToString(FileCategory::Code), "Code");
    EXPECT_STREQ(categoryToString(FileCategory::Other), "Other");
}

TEST(TypesTest, StringToCategory) {
    EXPECT_EQ(stringToCategory("Images"), FileCategory::Images);
    EXPECT_EQ(stringToCategory("Documents"), FileCategory::Documents);
    EXPECT_EQ(stringToCategory("Videos"), FileCategory::Videos);
    EXPECT_EQ(stringToCategory("Audio"), FileCategory::Audio);
    EXPECT_EQ(stringToCategory("Code"), FileCategory::Code);
    EXPECT_EQ(stringToCategory("Other"), FileCategory::Other);
    EXPECT_FALSE(stringToCategory("Invalid").has_value());
}

TEST(TypesTest, FileInfoConstruction) {
    fs::path testPath = "/test/path/image.JPG";
    FileInfo info(testPath);

    EXPECT_EQ(info.path, testPath);
    EXPECT_EQ(info.name, "image.JPG");
    EXPECT_EQ(info.extension, ".jpg"); // Should be lowercase
    EXPECT_EQ(info.category, FileCategory::Other); // Default
}

TEST(TypesTest, FileInfoExtensionNormalization) {
    FileInfo info1(fs::path("/test/FILE.PDF"));
    EXPECT_EQ(info1.extension, ".pdf");

    FileInfo info2(fs::path("/test/image.PNG"));
    EXPECT_EQ(info2.extension, ".png");

    FileInfo info3(fs::path("/test/noextension"));
    EXPECT_EQ(info3.extension, "");
}

TEST(TypesTest, OperationStatusToString) {
    EXPECT_STREQ(statusToString(OperationStatus::Success), "success");
    EXPECT_STREQ(statusToString(OperationStatus::Failed), "failed");
    EXPECT_STREQ(statusToString(OperationStatus::Skipped), "skipped");
    EXPECT_STREQ(statusToString(OperationStatus::DryRun), "dry_run");
}

TEST(TypesTest, OrganizationStatsSuccessRate) {
    OrganizationStats stats;
    stats.totalFiles = 100;
    stats.filesProcessed = 95;

    EXPECT_DOUBLE_EQ(stats.successRate(), 95.0);

    // Test zero files case
    OrganizationStats emptyStats;
    EXPECT_DOUBLE_EQ(emptyStats.successRate(), 100.0);
}

} // namespace testing
} // namespace fileforge
