// RandomXMiner.cpp
#include "RandomXMiner.h"
#include <iostream>

RandomXMiner::RandomXMiner() {
    initialize();
}

RandomXMiner::~RandomXMiner() {
    cleanup();
}

void RandomXMiner::initialize() {
    // Allocate RandomX dataset and cache
    cache = randomx_alloc_cache(RANDOMX_FLAG_DEFAULT);
    if (!cache) {
        throw std::runtime_error("Failed to allocate RandomX cache");
    }

    // Initialize the cache with a random seed or specific key
    const char* key = "random_seed";
    randomx_init_cache(cache, key, strlen(key));

    // Create a dataset
    dataset = randomx_alloc_dataset(RANDOMX_FLAG_DEFAULT);
    if (!dataset) {
        throw std::runtime_error("Failed to allocate RandomX dataset");
    }

    // Initialize the dataset
    randomx_init_dataset(dataset, cache, 0, randomx_dataset_item_count());

    // Create the VM
    vm = randomx_create_vm(RANDOMX_FLAG_DEFAULT, cache, dataset);
    if (!vm) {
        throw std::runtime_error("Failed to create RandomX VM");
    }
}

void RandomXMiner::cleanup() {
    if (vm) {
        randomx_destroy_vm(vm);
    }
    if (dataset) {
        randomx_release_dataset(dataset);
    }
    if (cache) {
        randomx_release_cache(cache);
    }
}

void RandomXMiner::mine() {
    char input[32] = {0};
    char output[32];

    randomx_calculate_hash(vm, input, sizeof(input), output);

    std::cout << "Mining result: " << std::hex << std::string(output, sizeof(output)) << std::endl;
}
