#include "real_file_system.h"
#include <sys/stat.h>
#include <fstream>
#include <cstdio>

bool RealFileSystem::createDirectory(const std::string& path) {
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
}

bool RealFileSystem::fileExists(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

long RealFileSystem::getFileSize(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 ? st.st_size : 0;
}

bool RealFileSystem::removeFile(const std::string& path) {
    return std::remove(path.c_str()) == 0;
}

bool RealFileSystem::renameFile(const std::string& from, const std::string& to) {
    return std::rename(from.c_str(), to.c_str()) == 0;
}

bool RealFileSystem::copyFile(const std::string& from, const std::string& to) {
    std::ifstream src(from, std::ios::binary);
    std::ofstream dst(to, std::ios::binary);
    return src.is_open() && dst.is_open() && (dst << src.rdbuf()).good();
}

bool RealFileSystem::writeToFile(const std::string& path, const std::string& content, bool append) {
    std::ofstream file(path, append ? std::ios::app : std::ios::trunc);
    return file.is_open() && (file << content << std::endl).good();
}
