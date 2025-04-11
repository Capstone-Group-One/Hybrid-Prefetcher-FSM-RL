
#include "classification_table.h"
#include <cstddef>
ClassificationTable classification_table;


bool ClassificationTable::has(uint64_t key) {
    return table.find(key) != table.end();
}

uint8_t ClassificationTable::get(uint64_t key) {
    if (!has(key)) return 0;
    lru_list.erase(key_positions[key]);
    lru_list.push_front(key);
    key_positions[key] = lru_list.begin();
    return table[key];
}

void ClassificationTable::insert(uint64_t key, uint8_t strategy) {
    if (has(key)) {
        table[key] = strategy;
        lru_list.erase(key_positions[key]);
        lru_list.push_front(key);
        key_positions[key] = lru_list.begin();
    } else {
        if (table.size() >= CLASSIFICATION_TABLE_SIZE) {
            uint64_t lru_key = lru_list.back();
            lru_list.pop_back();
            table.erase(lru_key);
            key_positions.erase(lru_key);
        }
        table[key] = strategy;
        lru_list.push_front(key);
        key_positions[key] = lru_list.begin();
    }
}

size_t ClassificationTable::size() const {
    return table.size();
}
