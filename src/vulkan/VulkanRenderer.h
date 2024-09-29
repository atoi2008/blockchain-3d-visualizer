// VulkanRenderer.h
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

class VulkanRenderer {
public:
VulkanRenderer(GLFWwindow* win); // Constructor
    void init();
    void cleanup();
    void render();
    void recreateSwapchain();
    void setFramebufferResizedFlag(bool resized);
    bool isRunning();
    GLFWwindow* getWindow();

private:
    void createInstance();
    void createSwapchain();
    void createImageViews();
    void createCommandBuffer();
    void cleanupSwapchain();
    bool checkValidationLayerSupport();

    // Vulkan members
    GLFWwindow* window; // Declare window
    VkInstance instance;
    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSwapchainKHR swapchain;
    VkSurfaceKHR surface; // Declare surface
    std::vector<VkImageView> swapchainImageViews;
    uint32_t imageIndex; // Declare imageIndex
    int graphicsQueueFamilyIndex; // Assume this is set correctly
    VkQueue graphicsQueue; // Declare graphicsQueue
    bool framebufferResized = false;
};

#endif // VULKAN_RENDERER_H
