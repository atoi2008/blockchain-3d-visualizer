// VulkanRenderer.h
#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

class VulkanRenderer {
public:
    void init();
    void render();
    void cleanup();
    bool isRunning();
    void recreateSwapchain();
    void setFramebufferResizedFlag(bool resized);
    GLFWwindow* getWindow();

private:
    // Vulkan components
    VkInstance instance;
    VkDevice device;
    VkSwapchainKHR swapChain;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    // Window and framebuffer
    GLFWwindow* window;
    bool framebufferResized = false;

    // Functions
    void createInstance();
    void createSwapchain();
    void createImageViews();
    void createFramebuffers();
    void cleanupSwapchain();
};

#endif // VULKANRENDERER_H
