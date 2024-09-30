//RandomXMiner.cpp
#include "RandomXMiner.h"
#include <iostream>

RandomXMiner::RandomXMiner() : vm(nullptr), dataset(nullptr), mining_active(false) {}

RandomXMiner::~RandomXMiner() {
    if (vm) randomx_destroy_vm(vm);
    if (dataset) randomx_release_dataset(dataset);
}

void RandomXMiner::initialize(const uint8_t* key, size_t key_size) {
    randomx_cache* cache = randomx_alloc_cache(RANDOMX_FLAG_DEFAULT); // Updated function name
    randomx_init_cache(cache, key, key_size);

    dataset = randomx_alloc_dataset(RANDOMX_FLAG_DEFAULT); // Updated function name
    randomx_init_dataset(dataset, cache, 0, randomx_dataset_item_count());
    randomx_release_cache(cache);

    vm = randomx_create_vm(RANDOMX_FLAG_DEFAULT, cache, dataset);
}

void RandomXMiner::mine() {
    mining_active = true;
    while (mining_active) {

        // TODO: Mining loop with RandomX hashing logic
        /*
            while (!isValidHash(block.hash)) {
            block.nonce++;
            block.hash = calculateHash(block);
        }
        std::lock_guard<std::mutex> lock(minedBlocksMutex);
        minedBlocks.push_back(block);

        */

        std::cout << "Mining in progress..." << std::endl;
        
    }
}

void RandomXMiner::stopMining() {
    mining_active = false;
    std::cout << "Mining stopped." << std::endl;
}
