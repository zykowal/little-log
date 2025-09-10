#pragma once
#include <string>

class Logger {
public:
    void init(const std::string& filePreName, const std::string& filePath, 
              int fileSize, int fileNum, const std::string& backupPath, int backupMaxSize);
    void enableTerminal();
    void disableTerminal();
    void logMsg(const std::string& message);
    void logFact(const std::string& message);
    void backup();

private:
    std::string filePreName_;
    std::string filePath_;
    int fileSize_;
    int fileNum_;
    std::string backupPath_;
    int backupMaxSize_;
    bool terminalEnabled_ = true;
    
    void createDirectories(const std::string& path);
    void rotateLogFiles();
    long getFileSize(const std::string& filename);
    void writeToFile(const std::string& message);
    void copyFile(const std::string& src, const std::string& dest);
    void cleanOldBackups();
    std::string getCurrentTimestamp();
};
