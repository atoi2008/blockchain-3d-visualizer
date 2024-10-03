//RandomXMinerTest.cpp
#include "RandomXMiner.h"
#include "Block.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// A simple function to simulate testing the RandomXMiner class
void testRandomXMiner() {
    const int DIFFICULTY = 4; // Define difficulty as a constant

    // Initialize the miner
    RandomXMiner miner;

    // Key for RandomX initialization (could be any arbitrary byte data)
    uint8_t key[32] = {0}; // Just using a zeroed-out key for simplicity
    size_t key_size = sizeof(key);

    try {
        // Initialize the RandomX miner with the key
        miner.initialize(key, key_size);
        std::cout << "RandomX miner initialized successfully." << std::endl;

        // Create a BlockHeader object with some sample values
        BlockHeader header(1, "00000000000000000000", "merkle_root_example", time(nullptr), 0, DIFFICULTY);

        // Start the mining process in a separate thread
        std::thread miningThread([&miner]() {
            miner.mine();
        });

        // Let the miner work for a while (e.g., 10 seconds) before stopping
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // Stop the mining process
        miner.stopMining();

        // Wait for the mining thread to finish
        if (miningThread.joinable()) {
            miningThread.join();
        }

        std::cout << "Mining process completed." << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main() {
    // Run the test function
    testRandomXMiner();
    return 0;
}
