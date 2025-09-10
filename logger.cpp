#include "logger.h"
#include "real_file_system.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

Logger::Logger(std::shared_ptr<FileSystemInterface> fs)
    : fs_(fs ? fs : std::make_shared<RealFileSystem>()) {}

void Logger::init(const std::string &filePreName, const std::string &filePath,
                  int fileSize, int fileNum, const std::string &backupPath,
                  int backupMaxSize) {
  filePreName_ = filePreName;
  filePath_ = filePath;
  fileSize_ = fileSize * 1024 * 1024;
  fileNum_ = fileNum;
  backupPath_ = backupPath;
  backupMaxSize_ = backupMaxSize * 1024 * 1024;

  createDirectories(filePath_ + "/" + filePreName_);
  createDirectories(backupPath_);
}

void Logger::enableTerminal() { terminalEnabled_ = true; }

void Logger::disableTerminal() { terminalEnabled_ = false; }

void Logger::logMsg(const std::string &message) {
  if (terminalEnabled_) {
    std::cout << message << std::endl;
  }
  writeToFile(message);
}

void Logger::logFact(const std::string &message) {
  std::cout << message << std::endl;
}

void Logger::backup() {
  std::string timestamp = getCurrentTimestamp();
  std::string backupDir =
      backupPath_ + "/" + filePreName_ + "/log_" + timestamp;
  createDirectories(backupDir);

  long totalSize = 0;
  for (int i = 0; i < fileNum_; ++i) {
    std::string sourceFile = getLogFilePath(i);
    if (fs_->fileExists(sourceFile)) {
      long fileSize = fs_->getFileSize(sourceFile);
      if (totalSize + fileSize <= backupMaxSize_) {
        std::string destFile = backupDir + "/" + filePreName_ +
                               (i == 0 ? ".log" : ".log" + std::to_string(i));
        fs_->copyFile(sourceFile, destFile);
        totalSize += fileSize;
      } else {
        break;
      }
    }
  }
}

void Logger::createDirectories(const std::string &path) {
  size_t pos = 0;
  while ((pos = path.find('/', pos)) != std::string::npos) {
    std::string dir = path.substr(0, pos++);
    if (!dir.empty()) {
      fs_->createDirectory(dir);
    }
  }
  fs_->createDirectory(path);
}

void Logger::rotateLogFiles() {
  std::string oldestFile = getLogFilePath(fileNum_ - 1);
  fs_->removeFile(oldestFile);

  for (int i = fileNum_ - 2; i >= 1; --i) {
    std::string currentFile = getLogFilePath(i);
    std::string nextFile = getLogFilePath(i + 1);
    if (fs_->fileExists(currentFile)) {
      fs_->renameFile(currentFile, nextFile);
    }
  }

  std::string mainFile = getLogFilePath(0);
  std::string firstBackup = getLogFilePath(1);
  if (fs_->fileExists(mainFile)) {
    fs_->renameFile(mainFile, firstBackup);
  }
}

void Logger::writeToFile(const std::string &message) {
  std::string logFile = getLogFilePath(0);

  if (fs_->getFileSize(logFile) + message.length() + 1 > fileSize_) {
    rotateLogFiles();
  }

  fs_->writeToFile(logFile, message, true);
}

std::string Logger::getCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t), "%Y%m%d%H%M%S");
  return ss.str();
}

std::string Logger::getLogFilePath(int index) const {
  std::string logDir = filePath_ + "/" + filePreName_;
  return logDir + "/" + filePreName_ +
         (index == 0 ? ".log" : ".log" + std::to_string(index));
}
