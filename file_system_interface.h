#pragma once
#include <string>

class FileSystemInterface {
public:
    virtual ~FileSystemInterface() = default;
    virtual bool createDirectory(const std::string& path) = 0;
    virtual bool fileExists(const std::string& path) = 0;
    virtual long getFileSize(const std::string& path) = 0;
    virtual bool removeFile(const std::string& path) = 0;
    virtual bool renameFile(const std::string& from, const std::string& to) = 0;
    virtual bool copyFile(const std::string& from, const std::string& to) = 0;
    virtual bool writeToFile(const std::string& path, const std::string& content, bool append = true) = 0;
};
