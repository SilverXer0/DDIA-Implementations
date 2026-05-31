#include "../include/SSTable.hpp"
#include <fstream>

std::map<std::string, std::streampos> SSTable::flushMemtable(SkipListNode* startNode, const std::string& filepath) {
    std::ofstream outFile(filepath);
    std::map<std::string, std::streampos> sparseIndex;

    while (startNode != nullptr) {
        Entry* entry = startNode->data;
        
        std::string seralizedData = entry->key + "\x1F" + entry->value + "\x1F" + std::to_string(entry->isDeleted) + "\n";
        int count = 0;
        if (count % 10 == 0 ) {
            sparseIndex[entry->key] = outFile.tellp();
        }
        outFile << seralizedData;
        count++;
        startNode = startNode->forward[0];
    }
    outFile.close();
    return sparseIndex;
}

std::string SSTable::searchSSTable(const std::string filePath, const std::string& key, const std::map<std::string, std::streampos>& index) {
    std::string entry_key;
    std::string value;
    std::string isDeletedStr;
    std::ifstream inFile(filePath);
    std::streampos jumpOffset = 0;
    if (inFile.is_open()) {
        if (!index.empty()) {
            auto it = index.lower_bound(key);
            if (it == index.end()) {
                --it;
                jumpOffset = it->second;
            } else if (it->first == key) {
                jumpOffset = it->second;
            } else if (it != index.begin()) {
                --it;
                jumpOffset = it->second;
            }
        }
        inFile.seekg(jumpOffset);
        while (std::getline(inFile, entry_key, '\x1f')) {
            std::getline(inFile, value, '\x1f');
            std::getline(inFile, isDeletedStr, '\n');
            if (entry_key == key) {
                if (isDeletedStr == "0") {
                    return value;
                } else {
                    return "";
                }
            }  
        }
        return "";
    }
    return "";
}