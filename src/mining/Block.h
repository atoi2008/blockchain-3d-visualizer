// Block.h
#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <cstdint>

struct BlockHeader {
    uint8_t version;
    std::string prev_block_hash;
    std::string merkle_root;
    uint64_t timestamp;
    uint64_t nonce;
    uint32_t difficulty_target;
    
    // Constructor to initialize values
    BlockHeader(uint8_t v, const std::string& prev_hash, const std::string& merkle, uint64_t time, uint64_t n, uint32_t diff)
        : version(v), prev_block_hash(prev_hash), merkle_root(merkle), timestamp(time), nonce(n), difficulty_target(diff) {}
};

#endif // BLOCK_H
