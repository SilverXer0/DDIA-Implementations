#include <iostream>
#include <string>
#include "../include/LSMTree.hpp"

// A quick helper to make keys sort correctly (e.g. "key_005" instead of "key_5")
std::string padKey(int i) {
    std::string s = std::to_string(i);
    return "key_" + std::string(5 - s.length(), '0') + s;
}

int main() {
    std::cout << "--- Starting LSM Tree Database ---" << std::endl;
    
    // maxLevel = 10, prob = 0.5
    LSMTree db(10, 0.5f, "test_wal.txt");
    
    std::cout << "Inserting 500 keys..." << std::endl;
    for (int i = 0; i < 500; i++) {
        db.put(padKey(i), "value_" + std::to_string(i));
    }
    
    std::cout << "Finished inserting!" << std::endl;
    
    // 1. Test Reading a value
    std::string testKey = padKey(150);
    std::cout << "\nRetrieving " << testKey << "..." << std::endl;
    std::cout << "Result: " << db.get(testKey) << std::endl;
    
    // 2. Test Deleting a value
    std::cout << "\nDeleting " << testKey << "..." << std::endl;
    db.del(testKey);
    std::cout << "Retrieving " << testKey << " after delete..." << std::endl;
    std::cout << "Result: " << db.get(testKey) << " (should be empty)" << std::endl;
    
    // 3. Test Updating a value
    std::string updateKey = padKey(300);
    std::cout << "\nUpdating " << updateKey << " to 'NEW_VALUE'..." << std::endl;
    db.put(updateKey, "NEW_VALUE");
    std::cout << "Result: " << db.get(updateKey) << std::endl;

    std::cout << "\n--- Simulation Complete ---" << std::endl;
    return 0;
}