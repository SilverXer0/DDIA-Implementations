#pragma once
#include <vector>
#include <string>
#include "Entry.hpp"

struct SkipListNode {
    Entry* data;
    int level;
    std::vector<SkipListNode*> forward;

    SkipListNode(Entry* entryData, int nodeLevel) :
        data(entryData), level(nodeLevel), forward(nodeLevel + 1, nullptr) {}
};

class SkipList {
    private:
        SkipListNode* header;
        int currentLevel;
        int maxLevel;
        float probability;

        int randomLevel();

    public:
        SkipList(int mxLvl, float prob);
        ~SkipList();

        bool insert(Entry *entry);
        Entry* search(const std::string& key);
};
