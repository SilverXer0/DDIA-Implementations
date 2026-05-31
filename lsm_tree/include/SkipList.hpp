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

// the forward vector contains pointers to other SkipListNodes: each node is nodeLevel levels high, which is
// how many levels this node participates in. This means that if our Node has level 2, it appears on 3 lanes. 
// If the node after this in the actual Skiplist only has level 1, then the 2nd level of the Node skips over it,
// pointing to the next node after that has level 2, or to a nullptr if none with a > value do.
// Meanwhile the first and 0 levels that do have something immediate to point to point to the node in each lane.

class SkipList {
    private:
        SkipListNode* header;
        int currentLevel;
        int maxLevel;
        float probability;
        size_t currentByteSize;
        size_t maxByteThreshold;

        int randomLevel();

    public:
        SkipList(int mxLvl, float prob);
        ~SkipList();

        bool insert(Entry *entry);
        Entry* search(const std::string& key);
        SkipListNode* getFirstNode() const;
        size_t getSize() const;
        size_t getThreshold() const;  
};
