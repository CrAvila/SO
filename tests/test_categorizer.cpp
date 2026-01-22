/**
 * @file test_categorizer.cpp
 * @brief Unit tests for FileForge categorizer
 */

#include <fileforge/categorizer.hpp>

#include <gtest/gtest.h>

namespace fileforge {
namespace testing {

class CategorizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config = std::make_unique<Config>();
        m_categorizer = std::make_unique<Categorizer>(*m_config);
    }

    std::unique_ptr<Config> m_config;
    std::unique_ptr<Categorizer> m_categorizer;
};

TEST_F(CategorizerTest, CategorizeImages) {
    EXPECT_EQ(m_categorizer->getCategory(".jpg"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".jpeg"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".png"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".gif"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".svg"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".webp"), FileCategory::Images);
}

TEST_F(CategorizerTest, CategorizeDocuments) {
    EXPECT_EQ(m_categorizer->getCategory(".pdf"), FileCategory::Documents);
    EXPECT_EQ(m_categorizer->getCategory(".doc"), FileCategory::Documents);
    EXPECT_EQ(m_categorizer->getCategory(".docx"), FileCategory::Documents);
    EXPECT_EQ(m_categorizer->getCategory(".txt"), FileCategory::Documents);
}

TEST_F(CategorizerTest, CategorizeVideos) {
    EXPECT_EQ(m_categorizer->getCategory(".mp4"), FileCategory::Videos);
    EXPECT_EQ(m_categorizer->getCategory(".mkv"), FileCategory::Videos);
    EXPECT_EQ(m_categorizer->getCategory(".avi"), FileCategory::Videos);
    EXPECT_EQ(m_categorizer->getCategory(".mov"), FileCategory::Videos);
}

TEST_F(CategorizerTest, CategorizeAudio) {
    EXPECT_EQ(m_categorizer->getCategory(".mp3"), FileCategory::Audio);
    EXPECT_EQ(m_categorizer->getCategory(".wav"), FileCategory::Audio);
    EXPECT_EQ(m_categorizer->getCategory(".flac"), FileCategory::Audio);
    EXPECT_EQ(m_categorizer->getCategory(".ogg"), FileCategory::Audio);
}

TEST_F(CategorizerTest, CategorizeCode) {
    EXPECT_EQ(m_categorizer->getCategory(".cpp"), FileCategory::Code);
    EXPECT_EQ(m_categorizer->getCategory(".hpp"), FileCategory::Code);
    EXPECT_EQ(m_categorizer->getCategory(".py"), FileCategory::Code);
    EXPECT_EQ(m_categorizer->getCategory(".js"), FileCategory::Code);
    EXPECT_EQ(m_categorizer->getCategory(".ts"), FileCategory::Code);
    EXPECT_EQ(m_categorizer->getCategory(".java"), FileCategory::Code);
    EXPECT_EQ(m_categorizer->getCategory(".rs"), FileCategory::Code);
}

TEST_F(CategorizerTest, CategorizeArchives) {
    EXPECT_EQ(m_categorizer->getCategory(".zip"), FileCategory::Archives);
    EXPECT_EQ(m_categorizer->getCategory(".rar"), FileCategory::Archives);
    EXPECT_EQ(m_categorizer->getCategory(".7z"), FileCategory::Archives);
    EXPECT_EQ(m_categorizer->getCategory(".tar"), FileCategory::Archives);
}

TEST_F(CategorizerTest, CategorizeUnknown) {
    EXPECT_EQ(m_categorizer->getCategory(".xyz"), FileCategory::Other);
    EXPECT_EQ(m_categorizer->getCategory(".unknown"), FileCategory::Other);
    EXPECT_EQ(m_categorizer->getCategory(""), FileCategory::Other);
}

TEST_F(CategorizerTest, CaseInsensitive) {
    EXPECT_EQ(m_categorizer->getCategory(".JPG"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".Jpg"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory(".PDF"), FileCategory::Documents);
    EXPECT_EQ(m_categorizer->getCategory(".MP4"), FileCategory::Videos);
}

TEST_F(CategorizerTest, ExtensionWithoutDot) {
    EXPECT_EQ(m_categorizer->getCategory("jpg"), FileCategory::Images);
    EXPECT_EQ(m_categorizer->getCategory("pdf"), FileCategory::Documents);
}

TEST_F(CategorizerTest, CategorizeFileInfo) {
    FileInfo info(fs::path("/test/image.jpg"));
    EXPECT_EQ(info.category, FileCategory::Other); // Default before categorization

    m_categorizer->categorize(info);
    EXPECT_EQ(info.category, FileCategory::Images);
}

TEST_F(CategorizerTest, CategorizeMultipleFiles) {
    std::vector<FileInfo> files = {
        FileInfo(fs::path("/test/image.jpg")),
        FileInfo(fs::path("/test/doc.pdf")),
        FileInfo(fs::path("/test/video.mp4")),
        FileInfo(fs::path("/test/code.cpp"))
    };

    m_categorizer->categorizeAll(files);

    EXPECT_EQ(files[0].category, FileCategory::Images);
    EXPECT_EQ(files[1].category, FileCategory::Documents);
    EXPECT_EQ(files[2].category, FileCategory::Videos);
    EXPECT_EQ(files[3].category, FileCategory::Code);
}

TEST_F(CategorizerTest, GroupByCategory) {
    std::vector<FileInfo> files = {
        FileInfo(fs::path("/test/image1.jpg")),
        FileInfo(fs::path("/test/image2.png")),
        FileInfo(fs::path("/test/doc.pdf")),
    };

    m_categorizer->categorizeAll(files);
    auto grouped = m_categorizer->groupByCategory(files);

    EXPECT_EQ(grouped[FileCategory::Images].size(), 2);
    EXPECT_EQ(grouped[FileCategory::Documents].size(), 1);
}

TEST_F(CategorizerTest, GetCategoryStats) {
    std::vector<FileInfo> files = {
        FileInfo(fs::path("/test/image1.jpg")),
        FileInfo(fs::path("/test/image2.png")),
        FileInfo(fs::path("/test/image3.gif")),
        FileInfo(fs::path("/test/doc.pdf")),
    };

    m_categorizer->categorizeAll(files);
    auto stats = m_categorizer->getCategoryStats(files);

    EXPECT_EQ(stats[FileCategory::Images], 3);
    EXPECT_EQ(stats[FileCategory::Documents], 1);
    EXPECT_EQ(stats[FileCategory::Videos], 0);
}

TEST_F(CategorizerTest, IsKnownExtension) {
    EXPECT_TRUE(m_categorizer->isKnownExtension(".jpg"));
    EXPECT_TRUE(m_categorizer->isKnownExtension(".pdf"));
    EXPECT_TRUE(m_categorizer->isKnownExtension(".mp4"));
    EXPECT_FALSE(m_categorizer->isKnownExtension(".xyz"));
}

} // namespace testing
} // namespace fileforge
