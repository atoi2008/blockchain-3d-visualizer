//RandomXMiner.cpp
#include "RandomXMiner.h"
#include <iostream>

RandomXMiner::RandomXMiner() : vm(nullptr), dataset(nullptr), mining_active(false) {}

RandomXMiner::~RandomXMiner() {
    if (vm) randomx_destroy_vm(vm);
    if (dataset) randomx_release_dataset(dataset);
}

void RandomXMiner::initialize(const uint8_t* key, size_t key_size) {
    randomx_cache* cache = randomx_create_cache(RANDOMX_FLAG_DEFAULT, key, key_size);
    randomx_init_cache(cache, key, key_size);

    dataset = randomx_allocate_dataset(RANDOMX_FLAG_DEFAULT);
    randomx_init_dataset(dataset, cache, 0, randomx_dataset_item_count());
    randomx_release_cache(cache);

    vm = randomx_create_vm(RANDOMX_FLAG_DEFAULT, cache, dataset);
}

void RandomXMiner::mine() {
    mining_active = true;
    while (mining_active) {
        // Mining loop with RandomX hashing logic
        std::cout << "Mining in progress..." << std::endl;
    }
}

void RandomXMiner::stopMining() {
    mining_active = false;
    std::cout << "Mining stopped." << std::endl;
}
