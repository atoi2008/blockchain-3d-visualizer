#ifndef RANDOM_X_MINER_H
#define RANDOM_X_MINER_H

#include "randomx.h"
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include "Block.h" 

class RandomXMiner {
public:
    RandomXMiner();
    ~RandomXMiner();

    void initialize(const uint8_t* key, size_t key_size);
    void mine();
    void stopMining();

private:
    std::string prepareInputData(const BlockHeader& header);
    std::string calculateHash(const BlockHeader& header); // Fixed the parameter type here
    bool isValidHash(const uint8_t *hash, size_t length);
    bool isValidHash(const uint8_t *hash, size_t length, int difficulty);

    randomx_vm* vm;
    randomx_dataset* dataset;
    bool mining_active;
    std::vector<std::string> minedBlocks;
    std::mutex minedBlocksMutex;
};

#endif // RANDOM_X_MINER_H
