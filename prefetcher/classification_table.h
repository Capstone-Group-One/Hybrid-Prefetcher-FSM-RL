
#ifndef CLASSIFICATION_TABLE_H
#define CLASSIFICATION_TABLE_H

#include <unordered_map>
#include <list>
#include <cstdint>
#include <cstddef>

#define CLASSIFICATION_TABLE_SIZE 1024

class ClassificationTable {
public:
    std::unordered_map<uint64_t, uint8_t> table;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> key_positions;
    std::list<uint64_t> lru_list;

    bool has(uint64_t key);
    uint8_t get(uint64_t key);
    void insert(uint64_t key, uint8_t strategy);
    size_t size() const;
};
extern ClassificationTable classification_table;

#endif // CLASSIFICATION_TABLE_H
