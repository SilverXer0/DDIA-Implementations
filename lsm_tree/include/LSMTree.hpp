#pragma once
#include "SkipList.hpp"
#include "WAL.hpp"
#include <map>
#include <string>

class LSMTree {
    private:
        SkipList* memtable; 
        WAL* wal;
        int maxLevel;
        float prob;
        std::string walPath;
        std::vector<std::string> sstableFiles;
        std::map<std::string, std::map<std::string, std::streampos>> sstableIndexes;

    public:
        LSMTree(int skipListMaxLevel, float skipListProb, const std::string walPath);
        ~LSMTree();

        void put(const std::string& key, const std::string& value);
        std::string get(const std::string& key);
        void del(const std::string& key);
        void compact();
};