
#ifndef FSM_PREFETCHER_H
#define FSM_PREFETCHER_H

#include <cstdint>
#include <deque>
#include <unordered_map>
#include "classification_table.h"

extern uint64_t fsm_prefetch_count;
extern uint64_t fsm_reward_at; // covered
extern uint64_t fsm_reward_in; // pollution

#define STRIDE_BASED     0x1
#define BRANCH_BASED     0x2
#define CACHE_BASED      0x4
#define NEXT_LINE        0x8
#define ADAPTIVE         0x10

#define FSM_CACHE_SIZE 64

struct FSMCacheEntry {
    uint64_t key;
    uint8_t strategy;
};

static std::deque<FSMCacheEntry> fsm_cache;

inline bool fsm_cache_lookup(uint64_t key, uint8_t &strategy_out) {
    for (auto &entry : fsm_cache) {
        if (entry.key == key) {
            strategy_out = entry.strategy;
            return true;
        }
    }
    return false;
}

inline void fsm_cache_insert(uint64_t key, uint8_t strategy) {
    if (fsm_cache.size() >= FSM_CACHE_SIZE)
        fsm_cache.pop_front();
    fsm_cache.push_back({key, strategy});
}

inline uint8_t get_delta_class(int64_t delta) {
    int64_t abs_delta = std::abs(delta);
    if (abs_delta == 0) return 0;
    if (abs_delta == 1) return 1;
    if (abs_delta <= 4) return 2;
    if (abs_delta <= 64) return 3;
    return 4;
}

inline uint64_t generate_fsm_key(uint64_t pc, int64_t delta, bool branch, bool miss) {
    uint64_t pc_hash = pc & 0xFFF;
    uint8_t delta_class = get_delta_class(delta);
    return (pc_hash << 8) | (delta_class << 4) | (branch << 1) | (miss);
}

inline uint8_t rule_based_fsm(uint64_t pc, int64_t delta, bool branch, bool miss) {
    if (delta == 1)
        return STRIDE_BASED;
    else if (branch && !miss)
        return BRANCH_BASED;
    else if (miss && delta < 4)
        return CACHE_BASED;
    else if (delta == 0)
        return NEXT_LINE;

    
    return ADAPTIVE;
}


extern ClassificationTable classification_table;

#endif // FSM_PREFETCHER_H
