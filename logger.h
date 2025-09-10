#pragma once
#include <string>
#include <memory>
#include "file_system_interface.h"

class Logger {
public:
    explicit Logger(std::shared_ptr<FileSystemInterface> fs = nullptr);
    void init(const std::string& filePreName, const std::string& filePath, 
              int fileSize, int fileNum, const std::string& backupPath, int backupMaxSize);
    void enableTerminal();
    void disableTerminal();
    void logMsg(const std::string& message);
    void logFact(const std::string& message);
    void backup();

private:
    std::shared_ptr<FileSystemInterface> fs_;
    std::string filePreName_;
    std::string filePath_;
    int fileSize_;
    int fileNum_;
    std::string backupPath_;
    int backupMaxSize_;
    bool terminalEnabled_ = true;
    
    void createDirectories(const std::string& path);
    void rotateLogFiles();
    void writeToFile(const std::string& message);
    std::string getCurrentTimestamp();
    std::string getLogFilePath(int index = 0) const;
};
