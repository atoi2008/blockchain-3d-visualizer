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

std::string RandomXMiner::prepareInputData(const BlockHeader& header) {
    std::ostringstream oss;
    oss << std::hex << header.nonce;  // Convert nonce to hexadecimal

    // Concatenate all fields of the block header to form the data to be hashed
    return std::to_string(header.version) +
           header.prev_block_hash +
           header.merkle_root +
           std::to_string(header.timestamp) +
           std::to_string(header.difficulty_target) +
           oss.str();  // Append nonce as part of the input data
}

std::string RandomXMiner::calculateHash(const BlockHeader& header) {
    std::string inputData = prepareInputData(header);  // Pass the header, not just the nonce

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

    // Initialize block header with nonce = 0
    BlockHeader header(1, "prev_hash", "merkle_root", time(nullptr), 0, DIFFICULTY);

    while (mining_active) {
        // Increment nonce in the block header
        header.nonce++; 

        // Prepare input data using the updated header
        std::string some_data = prepareInputData(header);

        uint8_t hash[RANDOMX_HASH_SIZE];
        randomx_calculate_hash(vm, some_data.data(), some_data.size(), hash);

        // Check if the hash is valid
        if (isValidHash(hash, RANDOMX_HASH_SIZE)) {
            std::lock_guard<std::mutex> lock(minedBlocksMutex);
            minedBlocks.push_back(calculateHash(header));  // Pass the entire header to calculateHash()
            std::cout << "Valid hash found: " << calculateHash(header) << std::endl;
            break;  // Stop mining after finding a valid hash
        }
    }
}

void RandomXMiner::stopMining() {
    mining_active = false;
    std::cout << "Mining stopped." << std::endl;
}
