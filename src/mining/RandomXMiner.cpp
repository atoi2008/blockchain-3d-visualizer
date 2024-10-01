#include "RandomXMiner.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <randomx.h>
#include <memory>
#include <stdexcept>

const int DIFFICULTY = 4; // Define difficulty as a constant

RandomXMiner::RandomXMiner() : vm(nullptr), dataset(nullptr), mining_active(false) {}

RandomXMiner::~RandomXMiner() {
    if (vm) randomx_destroy_vm(vm);
    if (dataset) randomx_release_dataset(dataset);
}

void RandomXMiner::initialize(const uint8_t* key, size_t key_size) {
    // Use unique_ptr for managing memory
    std::unique_ptr<randomx_cache, decltype(&randomx_release_cache)> cache(randomx_alloc_cache(RANDOMX_FLAG_DEFAULT), randomx_release_cache);
    if (!cache) {
        throw std::runtime_error("Failed to allocate RandomX cache!");
    }

    // Initialize the cache with the provided key
    randomx_init_cache(cache.get(), key, key_size);

    // Allocate the RandomX dataset
    dataset = randomx_alloc_dataset(RANDOMX_FLAG_DEFAULT);
    if (!dataset) {
        throw std::runtime_error("Failed to allocate RandomX dataset!");
    }

    // Initialize the dataset with the cache
    randomx_init_dataset(dataset, cache.get(), 0, randomx_dataset_item_count());

    // Create the RandomX virtual machine
    vm = randomx_create_vm(RANDOMX_FLAG_DEFAULT, cache.get(), dataset);
    if (!vm) {
        randomx_release_dataset(dataset); // Clean up dataset
        dataset = nullptr; // Prevent double free
        throw std::runtime_error("Failed to create RandomX virtual machine!");
    }
}

std::string RandomXMiner::prepareInputData(uint64_t nonce) {
    std::ostringstream oss;
    oss << std::hex << nonce; // Convert nonce to hexadecimal string
    return "block_header_data" + oss.str(); // Replace with actual block header data
}

std::string RandomXMiner::calculateHash(uint64_t nonce) {
    std::string inputData = prepareInputData(nonce); // Use the helper function

    uint8_t hash[RANDOMX_HASH_SIZE];
    randomx_calculate_hash(vm, inputData.c_str(), inputData.size(), hash);

    std::ostringstream hashStream;
    for (int i = 0; i < RANDOMX_HASH_SIZE; ++i) {
        hashStream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(hash[i]);
    }

    return hashStream.str();
}

bool RandomXMiner::isValidHash(const uint8_t* hash, size_t length) {
    std::ostringstream hashStream;
    for (size_t i = 0; i < length; ++i) {
        hashStream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(hash[i]);
    }
    std::string hashStr = hashStream.str();
    return hashStr.substr(0, DIFFICULTY) == std::string(DIFFICULTY, '0'); // Adjust based on your difficulty
}

void RandomXMiner::mine() {
    mining_active = true;
    uint64_t nonce = 0; // Initialize nonce

    while (mining_active) {
        std::string some_data = prepareInputData(nonce); // Use the helper function

        uint8_t hash[RANDOMX_HASH_SIZE];
        randomx_calculate_hash(vm, some_data.data(), some_data.size(), hash);

        // Check if the hash is valid
        if (isValidHash(hash, RANDOMX_HASH_SIZE)) { // Adjust difficulty here
            std::lock_guard<std::mutex> lock(minedBlocksMutex);
            minedBlocks.push_back(calculateHash(nonce)); // Store valid hash
            std::cout << "Valid hash found: " << calculateHash(nonce) << std::endl;
            break; // Exit loop or handle accordingly
        }

        nonce++; // Increment nonce

        // Optional: Implement nonce limit
        //if (nonce > MAX_NONCE) {
        //    std::cout << "Nonce limit reached. Stopping mining." << std::endl;
        //    break;
        //}
    }
}

void RandomXMiner::stopMining() {
    mining_active = false;
    std::cout << "Mining stopped." << std::endl;
}
