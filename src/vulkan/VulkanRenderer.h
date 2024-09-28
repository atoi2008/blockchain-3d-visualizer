// VulkanRenderer.h
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class VulkanRenderer {
public:
    VulkanRenderer();
    void init();  // Initialize Vulkan
    void render();  // Render the 3D environment
    void cleanup();  // Cleanup Vulkan resources
    void recreateSwapchain();  // Recreate swapchain on window resize
    bool isRunning();  // Check if the window is still open
    void setFramebufferResizedFlag(bool resized);  // Handle window resize flag
    GLFWwindow* getWindow();  // Get GLFW window handle

private:
    void createInstance();
    void createSwapchain();
    void createImageViews();
    void createFramebuffers();
    void createCommandBuffers();
    void cleanupSwapchain();

    GLFWwindow* window;
    VkInstance instance;
    VkSwapchainKHR swapChain;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkImageView> swapChainImageViews;
    VkDevice device;
    bool framebufferResized;
};
