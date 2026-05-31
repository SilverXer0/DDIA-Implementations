#include "../include/LSMTree.hpp"
#include "../include/SkipList.hpp"
#include "../include/SSTable.hpp"
#include "../include/WAL.hpp"

LSMTree::LSMTree(int skipListMaxLevel, float skipListProb, const std::string walPath) :
    maxLevel(skipListMaxLevel), prob(skipListProb), walPath(walPath) {
    wal = new WAL(walPath);
    memtable = new SkipList(skipListMaxLevel, skipListProb);
    std::ifstream inFile(walPath);
    std::string key;
    std::string value;
    std::string isDeletedStr;
    if (inFile.is_open()) {
        while (std::getline(inFile, key, '\x1f')) {
            std::getline(inFile, value, '\x1f');
            std::getline(inFile, isDeletedStr, '\n');
            Entry* entry = new Entry(key, value);
            entry->isDeleted = (isDeletedStr == "1");
            memtable->insert(entry);
        }
        inFile.close();
    }
    
}

LSMTree::~LSMTree() {
    delete wal;
    delete memtable;
}

void LSMTree::put(const std::string& key, const std::string& value) {
    Entry* entry = new Entry(key, value);
    wal->append(entry);
    memtable->insert(entry);

    if (memtable->getSize() > memtable->getThreshold()) {
        static int sstCounter = 0;
        std::string sstName = "sstable_" + std::to_string(sstCounter++) + ".sst";
        std::map<std::string, std::streampos> sstableIndex = SSTable::flushMemtable(memtable->getFirstNode(), sstName);
        sstableIndexes[sstName] = sstableIndex;
        LSMTree::sstableFiles.push_back(sstName);

        if (sstableFiles.size() >= 2) {
            compact();
        }
        delete memtable;
        delete wal;
        std::remove(walPath.c_str());

        memtable = new SkipList(maxLevel, prob);
        wal = new WAL(walPath);

    }
}

std::string LSMTree::get(const std::string& key) {
    Entry* entry = memtable->search(key);
    if (entry != nullptr) {
        if (entry->isDeleted) {
            return "";
        }
        return entry->value;
    }
    
    for (auto i = sstableFiles.rbegin(); i != sstableFiles.rend(); i++) {
        std::string result = SSTable::searchSSTable(*i, key, sstableIndexes[*i]);
        if (result != "") {
            return result;
        }
    }
    return "";
}

void LSMTree::del(const std::string& key) {
    Entry* tombstone = new Entry(key, "");
    tombstone->isDeleted = true;

    wal->append(tombstone);
    memtable->insert(tombstone);
}

void LSMTree::compact() {
    std::ifstream file1(sstableFiles[0]);
    std::ifstream file2(sstableFiles[1]);
    if (file1.is_open() && file2.is_open()) {
        static int mergeCounter = 0;
        std::string mergedName = "merged_" + std::to_string(mergeCounter++) + ".sst";
        std::ofstream outFile(mergedName);
        
        std::map<std::string, std::streampos> sparseIndex;
        int count = 0;
        
        auto writeEntry = [&](const std::string& k, const std::string& v, const std::string& d) {
            if (count % 10 == 0) {
                sparseIndex[k] = outFile.tellp();
            }
            outFile << k << "\x1F" << v << "\x1F" << d << "\n";
            count++;
        };

        std::string k1, v1, d1, k2, v2, d2;

        bool has1 = (bool)std::getline(file1, k1, '\x1F');
        if (has1) {
            std::getline(file1, v1, '\x1F');
            std::getline(file1, d1, '\n');
        }
        bool has2 = (bool)std::getline(file2, k2, '\x1F');
        if (has2) {
            std::getline(file2, v2, '\x1F');
            std::getline(file2, d2, '\n');
        }

        while (has1 && has2) {
            if (k1 < k2) {
                writeEntry(k1, v1, d1);
                has1 = (bool)std::getline(file1, k1, '\x1F');
                if (has1) { std::getline(file1, v1, '\x1F'); std::getline(file1, d1, '\n'); }
            } else if (k2 < k1) {
                writeEntry(k2, v2, d2);
                has2 = (bool)std::getline(file2, k2, '\x1F');
                if (has2) { std::getline(file2, v2, '\x1F'); std::getline(file2, d2, '\n'); }
            } else {
                if (d2 == "0") {
                    writeEntry(k2, v2, d2);
                }
                has1 = (bool)std::getline(file1, k1, '\x1F');
                if (has1) { std::getline(file1, v1, '\x1F'); std::getline(file1, d1, '\n'); }
                has2 = (bool)std::getline(file2, k2, '\x1F');
                if (has2) { std::getline(file2, v2, '\x1F'); std::getline(file2, d2, '\n'); }
            }
        }

        while (has1) {
            writeEntry(k1, v1, d1);
            has1 = (bool)std::getline(file1, k1, '\x1F');
            if (has1) { std::getline(file1, v1, '\x1F'); std::getline(file1, d1, '\n'); }
        }
        while (has2) {
            writeEntry(k2, v2, d2);
            has2 = (bool)std::getline(file2, k2, '\x1F');
            if (has2) { std::getline(file2, v2, '\x1F'); std::getline(file2, d2, '\n'); }
        }

        file1.close();
        file2.close();
        outFile.close();

        sstableIndexes.erase(sstableFiles[0]);
        sstableIndexes.erase(sstableFiles[1]);

        std::remove(sstableFiles[0].c_str());
        std::remove(sstableFiles[1].c_str());

        sstableFiles.clear();
        sstableFiles.push_back(mergedName);
        
        sstableIndexes[mergedName] = sparseIndex;
    }
}
