#pragma once
#include <string>
#include <map>
#include "SkipList.hpp"

class SSTable {
    public:
        static std::map<std::string, std::streampos> flushMemtable(SkipListNode* startNode, const std::string& filepath);
        static std::string searchSSTable(const std::string filePath, const std::string& key, const std::map<std::string, std::streampos>& index);
};

