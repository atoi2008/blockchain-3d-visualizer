#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init();
    void render();
    void cleanup();
    bool isRunning() const;

private:
    GLFWwindow* window;
    VkInstance instance;
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkFramebuffer framebuffer;
    VkPhysicalDevice physicalDevice;
    

    bool running;
    
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    void createInstance();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createGraphicsPipeline();
    void createCommandPool();
    void createCommandBuffer();
    void createFramebuffer();
    void drawFrame();
};

#endif // VULKAN_RENDERER_H
