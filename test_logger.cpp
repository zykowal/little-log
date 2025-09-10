#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "logger.h"
#include "real_file_system.h"
#include "file_system_interface.h"
#include <sstream>

class MockFileSystem : public FileSystemInterface {
public:
    MOCK_METHOD(bool, createDirectory, (const std::string& path), (override));
    MOCK_METHOD(bool, fileExists, (const std::string& path), (override));
    MOCK_METHOD(long, getFileSize, (const std::string& path), (override));
    MOCK_METHOD(bool, removeFile, (const std::string& path), (override));
    MOCK_METHOD(bool, renameFile, (const std::string& from, const std::string& to), (override));
    MOCK_METHOD(bool, copyFile, (const std::string& from, const std::string& to), (override));
    MOCK_METHOD(bool, writeToFile, (const std::string& path, const std::string& content, bool append), (override));
};

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockFs = std::make_shared<MockFileSystem>();
        logger = std::make_unique<Logger>(mockFs);
    }
    
    std::shared_ptr<MockFileSystem> mockFs;
    std::unique_ptr<Logger> logger;
};

class RealFileSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        realFs = std::make_shared<RealFileSystem>();
        testDir = "/tmp/logger_test";
        system(("rm -rf " + testDir).c_str());
    }
    
    void TearDown() override {
        system(("rm -rf " + testDir).c_str());
    }
    
    std::shared_ptr<RealFileSystem> realFs;
    std::string testDir;
};

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "/tmp/logger_integration_test";
        system(("rm -rf " + testDir).c_str());
        logger = std::make_unique<Logger>();
    }
    
    void TearDown() override {
        system(("rm -rf " + testDir).c_str());
    }
    
    std::unique_ptr<Logger> logger;
    std::string testDir;
};

// Mock tests
TEST_F(LoggerTest, InitCreatesDirectories) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .Times(::testing::AtLeast(2))
        .WillRepeatedly(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
}

TEST_F(LoggerTest, EnableDisableTerminal) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->enableTerminal();
    logger->disableTerminal();
}

TEST_F(LoggerTest, LogMsgWritesToFile) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, getFileSize(::testing::_))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*mockFs, writeToFile(::testing::_, "test message", true))
        .WillOnce(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->logMsg("test message");
}

TEST_F(LoggerTest, LogMsgTriggersRotationWhenFileTooLarge) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, getFileSize(::testing::_))
        .WillOnce(::testing::Return(2 * 1024 * 1024));
    EXPECT_CALL(*mockFs, removeFile(::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*mockFs, fileExists(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, renameFile(::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, writeToFile(::testing::_, "large message", true))
        .WillOnce(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->logMsg("large message");
}

TEST_F(LoggerTest, BackupCopiesFiles) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, fileExists(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, getFileSize(::testing::_))
        .WillRepeatedly(::testing::Return(1024));
    EXPECT_CALL(*mockFs, copyFile(::testing::_, ::testing::_))
        .Times(5)
        .WillRepeatedly(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->backup();
}

TEST_F(LoggerTest, BackupStopsWhenSizeLimitReached) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, fileExists(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, getFileSize(::testing::_))
        .WillRepeatedly(::testing::Return(6 * 1024 * 1024));
    EXPECT_CALL(*mockFs, copyFile(::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->backup();
}

TEST_F(LoggerTest, BackupSkipsNonExistentFiles) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, fileExists(::testing::_))
        .WillRepeatedly(::testing::Return(false));
    EXPECT_CALL(*mockFs, copyFile(::testing::_, ::testing::_))
        .Times(0);
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->backup();
}

TEST_F(LoggerTest, RotationWithMissingFiles) {
    EXPECT_CALL(*mockFs, createDirectory(::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockFs, getFileSize(::testing::_))
        .WillOnce(::testing::Return(2 * 1024 * 1024));
    EXPECT_CALL(*mockFs, removeFile(::testing::_))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*mockFs, fileExists(::testing::_))
        .WillRepeatedly(::testing::Return(false));
    EXPECT_CALL(*mockFs, renameFile(::testing::_, ::testing::_))
        .Times(0);
    EXPECT_CALL(*mockFs, writeToFile(::testing::_, ::testing::_, true))
        .WillOnce(::testing::Return(true));
    
    logger->init("test", "./logs", 1, 5, "./backup", 10);
    logger->logMsg("message");
}

// Real file system tests
TEST_F(RealFileSystemTest, CreateDirectory) {
    EXPECT_TRUE(realFs->createDirectory(testDir));
    EXPECT_TRUE(realFs->createDirectory(testDir)); // Should succeed even if exists
}

TEST_F(RealFileSystemTest, FileExists) {
    EXPECT_FALSE(realFs->fileExists(testDir + "/nonexistent.txt"));
    realFs->createDirectory(testDir);
    realFs->writeToFile(testDir + "/test.txt", "content", false);
    EXPECT_TRUE(realFs->fileExists(testDir + "/test.txt"));
}

TEST_F(RealFileSystemTest, GetFileSize) {
    realFs->createDirectory(testDir);
    EXPECT_EQ(0, realFs->getFileSize(testDir + "/nonexistent.txt"));
    realFs->writeToFile(testDir + "/test.txt", "hello", false);
    EXPECT_GT(realFs->getFileSize(testDir + "/test.txt"), 0);
}

TEST_F(RealFileSystemTest, WriteToFile) {
    realFs->createDirectory(testDir);
    EXPECT_TRUE(realFs->writeToFile(testDir + "/test.txt", "hello", false));
    EXPECT_TRUE(realFs->writeToFile(testDir + "/test.txt", "world", true));
}

TEST_F(RealFileSystemTest, CopyFile) {
    realFs->createDirectory(testDir);
    realFs->writeToFile(testDir + "/source.txt", "content", false);
    EXPECT_TRUE(realFs->copyFile(testDir + "/source.txt", testDir + "/dest.txt"));
    EXPECT_TRUE(realFs->fileExists(testDir + "/dest.txt"));
}

TEST_F(RealFileSystemTest, RenameFile) {
    realFs->createDirectory(testDir);
    realFs->writeToFile(testDir + "/old.txt", "content", false);
    EXPECT_TRUE(realFs->renameFile(testDir + "/old.txt", testDir + "/new.txt"));
    EXPECT_FALSE(realFs->fileExists(testDir + "/old.txt"));
    EXPECT_TRUE(realFs->fileExists(testDir + "/new.txt"));
}

TEST_F(RealFileSystemTest, RemoveFile) {
    realFs->createDirectory(testDir);
    realFs->writeToFile(testDir + "/test.txt", "content", false);
    EXPECT_TRUE(realFs->removeFile(testDir + "/test.txt"));
    EXPECT_FALSE(realFs->fileExists(testDir + "/test.txt"));
}

// Integration tests
TEST_F(IntegrationTest, FullWorkflow) {
    logger->init("test", testDir, 1, 3, testDir + "/backup", 10);
    
    // Test logging
    logger->logMsg("Message 1");
    logger->logMsg("Message 2");
    logger->disableTerminal();
    logger->logMsg("Message 3");
    logger->logFact("Force message");
    
    // Test backup
    logger->backup();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
