#include "miner_utils.h"
#include <chrono>

uint64_t miner_utils::getSystemTime() {
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}
