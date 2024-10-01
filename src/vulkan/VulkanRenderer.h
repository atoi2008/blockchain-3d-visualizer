#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanRenderer {
public:
    VulkanRenderer(GLFWwindow* win);
    void init();
    void cleanup();
    bool isRunning();
    void recreateSwapchain();
    void setFramebufferResizedFlag(bool resized);
    void render();
    GLFWwindow* getWindow();

private:
    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void cleanupSwapchain();
    void createCommandBuffer();
    bool checkValidationLayerSupport();
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device); 

    GLFWwindow* window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapchain;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface;  // Declare surface member variable

    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;

    bool framebufferResized = false;
};

#endif // VULKAN_RENDERER_H
