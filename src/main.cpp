// main.cpp
#include <iostream>
#include <thread>
#include <atomic>
#include <GLFW/glfw3.h> // Include GLFW for window handling
#include "vulkan/VulkanRenderer.h"
#include "mining/RandomXMiner.h"


// Flag to check if the window was resized
bool framebufferResized = false;

// Window Handling - Resize Callback
void framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto renderer = reinterpret_cast<VulkanRenderer *>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        framebufferResized = true;                 // Mark that the framebuffer was resized
        renderer->setFramebufferResizedFlag(true); // Inform the Vulkan renderer of the resize
    }
}

// Shared variables for mining and rendering
std::mutex dataMutex;
BlockHeader blockHeader;  // The block data that will be mined and visualized
bool miningActive = true; // Flag to control the mining thread

void miningThreadFunction()
{
    RandomXMiner miner;

    // Prepare initial data for mining
    blockHeader = miner.prepareInputData();

    while (miningActive)
    {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            // Mine and update the blockHeader
            miner.mine(blockHeader);
            // You can perform other logic here, like checking for success or progress updates
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Sleep to avoid overloading the CPU
    }
}

int main()
{

    std::cout << "Startup\n";

    // Initialize GLFW
    if (!glfwInit())
    {
        return -1;
    }

    // Set GLFW window hints for Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create the GLFW window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Blockchain 3D Visualizer", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Initialize Vulkan renderer
    VulkanRenderer renderer(window); // Pass the window to the VulkanRenderer constructor
    renderer.init();

    // Set user pointer for the window callback
    glfwSetWindowUserPointer(window, &renderer);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // Start the mining thread
    std::thread miningThread(miningThreadFunction);

    // Main application loop for rendering
    try
    {
        while (renderer.isRunning())
        {
            glfwPollEvents(); // Poll for events

            // Check if the framebuffer has been resized
            if (framebufferResized)
            {
                framebufferResized = false;
                renderer.recreateSwapchain(); // Recreate the swapchain on resize
            }
            // TODO: update the rendering based on mining results here
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                // Update the renderer with the latest blockHeader data from the miner
                renderer.update(blockHeader);
            }

            renderer.render(); // Render the 3D environment

            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate ~60 FPS
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Rendering error: " << e.what() << std::endl;
    }

 
    // Cleanup
    miningActive = false;
    miningThread.join(); // Ensure the mining thread completes
    renderer.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
