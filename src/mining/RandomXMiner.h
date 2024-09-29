// RandomXMiner.h
#ifndef RANDOMX_MINER_H
#define RANDOMX_MINER_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "randomx.h" // Ensure you include the correct path
#include "intrin_portable.h" // Assuming this is in the same folder structure

class RandomXMiner {
public:
    RandomXMiner();
    ~RandomXMiner();

    void mine();

private:
    randomx_cache* cache;
    randomx_vm* vm;
    randomx_dataset* dataset;

    void initialize();
    void cleanup();
};

#endif // RANDOMX_MINER_H
