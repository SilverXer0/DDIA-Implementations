#include "../include/SkipList.hpp"
#include <cstdlib>
#include <ctime>

// Tie function to class with SkipList::
// Use Initializer List to set simple variables
SkipList::SkipList(int mxLvl, float prob) : 
    currentLevel(0), maxLevel(mxLvl), probability(prob) {

        std::srand(std::time(nullptr));
        
        // Make DummyEntry 
        // new keyword asks OS for memory and returns a pointer to it
        Entry* dummyEntry = new Entry("", "");

        // make dummy node that gets dummy entry and build it to max level
        header = new SkipListNode(dummyEntry, maxLevel);

    }

int SkipList::randomLevel() {
    int level = 0;
    while (level < maxLevel - 1 && ((float)std::rand() / RAND_MAX) < probability) {
        level++;
    }
    return level;
}

bool SkipList::insert(Entry* entry) {
    // update is a vertical snapshot, recording one node per level
    // it records the last node we touched on that level before dropping down to a lower path on the skip list (more node path)
    // which is the last node on that level that is less than the value we are inserting so we know where to insert
    std::vector<SkipListNode*> update(maxLevel, nullptr);
    SkipListNode* current = header;
    // itereate through levels of skiplist from top to bottom
    for (int i = currentLevel; i >= 0; i--) {
        // only move current within the level if the current's data is less than the value we are inserting (to get the greatest node that is less)
        while (current->forward[i] != nullptr && current->forward[i]->data->key < entry->key) {
            current = current->forward[i];
        }
        // record the exact greatest smallest node
        update[i] = current;
    }
    // move one forward in the lowest lane to ensure the key isn't in the skiplist already (later step)
    current = current->forward[0];

    // if key exists, replace its value and isDeleted value, free memory, return false
    if (current != nullptr && current->data->key == entry->key) {
         current->data->value = entry->value;
         current->data->isDeleted = entry->isDeleted;
         
         delete entry;
         return false;
    }

    int newLevel = randomLevel();

    // if our new rolled level is > the current record for our level, we have to update our array for the new level
    // No other nodes exist at this height except the dummy header, so we have to fill out the new levels with our 
    // dummy node so we can then connect the new node at these higher levels to the dummy node 
    // we're just adding the header (dummy node) to the blank levels so we have something to attach to. 
    if (newLevel > currentLevel) {
        for (int i = currentLevel + 1; i <= newLevel; i++) {
            update[i] = header;
        }
        currentLevel = newLevel;
    }

    // if it isn't a new record, we make the new node, and for every level, we update the new node to point to 
    // what our greatest lesser node was pointing to and then make that node point to our new node (LL insertion)
    SkipListNode* newNode = new SkipListNode(entry, newLevel);

    for (int i = 0; i <= newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
    return true;
}

Entry* SkipList::search(const std::string& key) {
    // similar to insert: set a current pointer to header, iterate through levels and find greatest smaller value,
    // go down a level and keep going, do one more at the end so ur either at a node greater or the node of the right key, 
    // and then check if it is correct
    SkipListNode* current = header;
    for (int i = currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->data->key < key) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];

    if (current != nullptr && current->data->key == key && !current->data->isDeleted) {
        return current->data;
    } 
    return nullptr;
}
