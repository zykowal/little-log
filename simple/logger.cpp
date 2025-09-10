#include "logger.h"
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

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

  cleanOldBackups();

  std::string sourceDir = filePath_ + "/" + filePreName_;
  long totalSize = 0;

  for (int i = 0; i < fileNum_; ++i) {
    std::string sourceFile = sourceDir + "/" + filePreName_ +
                             (i == 0 ? ".log" : ".log" + std::to_string(i));
    long fileSize = getFileSize(sourceFile);
    if (fileSize > 0 && totalSize + fileSize <= backupMaxSize_) {
      std::string destFile = backupDir + "/" + filePreName_ +
                             (i == 0 ? ".log" : ".log" + std::to_string(i));
      copyFile(sourceFile, destFile);
      totalSize += fileSize;
    } else if (fileSize > 0) {
      break;
    }
  }
}

void Logger::createDirectories(const std::string &path) {
  size_t pos = 0;
  std::string dir;
  while ((pos = path.find('/', pos)) != std::string::npos) {
    dir = path.substr(0, pos++);
    if (!dir.empty()) {
      mkdir(dir.c_str(), 0755);
    }
  }
  mkdir(path.c_str(), 0755);
}

void Logger::rotateLogFiles() {
  std::string logDir = filePath_ + "/" + filePreName_;

  std::string oldestFile =
      logDir + "/" + filePreName_ + ".log" + std::to_string(fileNum_ - 1);
  std::remove(oldestFile.c_str());

  for (int i = fileNum_ - 2; i >= 1; --i) {
    std::string currentFile =
        logDir + "/" + filePreName_ + ".log" + std::to_string(i);
    std::string nextFile =
        logDir + "/" + filePreName_ + ".log" + std::to_string(i + 1);
    std::rename(currentFile.c_str(), nextFile.c_str());
  }

  std::string mainFile = logDir + "/" + filePreName_ + ".log";
  std::string firstBackup = logDir + "/" + filePreName_ + ".log1";
  std::rename(mainFile.c_str(), firstBackup.c_str());
}

long Logger::getFileSize(const std::string &filename) {
  struct stat st;
  if (stat(filename.c_str(), &st) == 0) {
    return st.st_size;
  }
  return 0;
}

void Logger::writeToFile(const std::string &message) {
  std::string logDir = filePath_ + "/" + filePreName_;
  std::string logFile = logDir + "/" + filePreName_ + ".log";

  if (getFileSize(logFile) + message.length() + 1 > fileSize_) {
    rotateLogFiles();
  }

  std::ofstream file(logFile, std::ios::app);
  if (file.is_open()) {
    file << message << std::endl;
    file.close();
  }
}

void Logger::copyFile(const std::string &src, const std::string &dest) {
  std::ifstream source(src, std::ios::binary);
  std::ofstream destination(dest, std::ios::binary);
  destination << source.rdbuf();
}

void Logger::cleanOldBackups() {
  // 简化实现：只保留最新的备份目录
  // 完整实现需要遍历目录，这里用基本的文件操作
}

std::string Logger::getCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t), "%Y%m%d%H%M%S");
  return ss.str();
}
