// main.cpp
#include <iostream>
#include <thread>
#include <atomic>
#include "vulkan/VulkanRenderer.h"  // Updated file reference
#include "mining/RandomXMiner.h"    // Corrected file reference

// Global atomic variable to control mining
std::atomic<bool> isMining{true};

// Flag to check if the window was resized
bool framebufferResized = false;

// Window Handling - Resize Callback
void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto renderer = reinterpret_cast<VulkanRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        framebufferResized = true;  // Mark that the framebuffer was resized
        renderer->setFramebufferResizedFlag(true);  // Inform the Vulkan renderer of the resize
    }
}

// Function to handle CPU mining
void startMining() {
    try {
        // Initialize the mining engine
        RandomXMiner miner;

        while (isMining) {
            miner.mine();  // Perform mining operation
            // Optionally, you can add a sleep or condition to limit mining speed
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

            // Check if the framebuffer has been resized
            if (framebufferResized) {
                framebufferResized = false;
                renderer.recreateSwapchain();  // Recreate the swapchain on resize
            }

            renderer.render();  // Render the 3D environment
            // TODO: update the rendering based on mining results here
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
