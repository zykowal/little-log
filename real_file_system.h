#pragma once
#include "file_system_interface.h"

class RealFileSystem : public FileSystemInterface {
public:
    bool createDirectory(const std::string& path) override;
    bool fileExists(const std::string& path) override;
    long getFileSize(const std::string& path) override;
    bool removeFile(const std::string& path) override;
    bool renameFile(const std::string& from, const std::string& to) override;
    bool copyFile(const std::string& from, const std::string& to) override;
    bool writeToFile(const std::string& path, const std::string& content, bool append = true) override;
};
