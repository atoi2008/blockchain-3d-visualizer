// VulkanRenderer.h
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

class VulkanRenderer {
public:
    void init();
    void cleanup();
    void render();
    void recreateSwapchain();
    void setFramebufferResizedFlag(bool resized);
    GLFWwindow* getWindow();
    bool isRunning();  // Ensure this is declared here

private:
    void createInstance();
    void createSwapchain();
    void createImageViews();
    void cleanupSwapchain();
    void createCommandBuffer();
    
    VkInstance instance;
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    GLFWwindow* window;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface; // Vulkan surface for rendering
    uint32_t graphicsQueueFamilyIndex; // Queue family index
    uint32_t imageIndex; // Current image index to be presented

    bool framebufferResized = false;
    std::vector<VkImageView> swapchainImageViews;
};

#endif // VULKAN_RENDERER_H
