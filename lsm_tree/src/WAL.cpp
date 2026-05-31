#include "../include/WAL.hpp"
#include <string>

WAL::WAL(const std::string& filePath) : 
logFilePath(filePath),
fileStream(filePath, std::ios::app) {}

WAL::~WAL() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void WAL::append(const Entry* entry) {
    std::lock_guard<std::mutex> lock(writeMutex);
    std::string seralizedData = entry->key + "\x1F" + entry->value + "\x1F" + std::to_string(entry->isDeleted) + "\n";
    fileStream << seralizedData;
    fileStream.flush();
}