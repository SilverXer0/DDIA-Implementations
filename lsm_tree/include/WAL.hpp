#pragma once
#include <fstream>
#include <string>
#include "Entry.hpp"
#include <mutex>


class WAL {
    private: 
        std::string logFilePath;
        std::ofstream fileStream;
        std::mutex writeMutex;

    public:
        WAL(const std::string& filePath);
        ~WAL();
        void append(const Entry* entry);
};
