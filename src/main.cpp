//main.cpp
#include <iostream>
#include <thread>
#include <atomic>
#include "vulkan/vulkan_renderer.h"  // Include your Vulkan renderer header
#include "mining/mining.h"           // Include your mining header

// Global atomic variable to control mining
std::atomic<bool> isMining{true};

// Window Handling
void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<VulkanRenderer*>(glfwGetWindowUserPointer(window));
    if (app) {
        // Handle resizing (this would typically recreate the swapchain)
    }
}

// Function to handle CPU mining
void startMining() {
    try {
        // Initialize the mining engine
        RandomXMiner miner;

        while (isMining) {
            miner.mine();  // Perform mining operation
            // Optionally, you can add a sleep or some condition to limit mining speed
        }
    } catch (const std::exception& e) {
        std::cerr << "Mining error: " << e.what() << std::endl;
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Set GLFW window hints for Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Initialize Vulkan renderer
    VulkanRenderer renderer;
    renderer.init();

    // Set user pointer for the window callback
    glfwSetWindowUserPointer(renderer.getWindow(), &renderer);
    glfwSetFramebufferSizeCallback(renderer.getWindow(), framebufferResizeCallback);


    // Start the mining thread
    std::thread miningThread(startMining);

    // Main application loop for rendering
    try {
        while (renderer.isRunning()) { 
            glfwPollEvents();  // Poll for events
            renderer.render();  // Render the 3D environment
            //TODO: update the rendering based on mining results here
        }
    } catch (const std::exception& e) {
        std::cerr << "Rendering error: " << e.what() << std::endl;
    }

    // Stop mining and join the thread before exiting
    isMining = false;
    miningThread.join();

    // Clean up Vulkan resources
    renderer.cleanup();

    return 0;
}
