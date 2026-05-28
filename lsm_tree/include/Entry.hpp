#pragma once
#include <string>

struct Entry {
    std::string key;
    std::string value;
    bool isDeleted;

    Entry(std::string k, std::string v) : key(k), value(v), isDeleted(false) {}
};