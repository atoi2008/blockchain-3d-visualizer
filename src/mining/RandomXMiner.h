#ifndef RANDOMX_MINER_H
#define RANDOMX_MINER_H

#include "randomx.h"

class RandomXMiner {
public:
    RandomXMiner();
    ~RandomXMiner();

    void initialize(const uint8_t* key, size_t key_size);
    void mine();
    void stopMining();

private:
    randomx_vm* vm;
    randomx_dataset* dataset;
    bool mining_active;
};

#endif // RANDOMX_MINER_H
