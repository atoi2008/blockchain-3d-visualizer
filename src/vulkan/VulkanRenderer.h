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

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer {
public:
    VulkanRenderer(GLFWwindow* win);
    void init();
    void cleanup();
    bool isRunning();
    void recreateSwapchain();
    void setFramebufferResizedFlag(bool resized);
    void checkFramebufferResized();
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
    void createRenderPass();
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

    // Helper functions for swap chain
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    std::vector<VkImage> swapchainImages; // To hold the swapchain images
    std::vector<VkImageView> swapchainImageViews; // To hold the image views
    std::vector<VkFramebuffer> swapchainFramebuffers; // Framebuffers for the swapchain images
    VkClearValue clearColor; // Clear color value for the render pass
    VkFormat swapchainImageFormat; // To hold the format of the swapchain images
    VkSemaphore imageAvailableSemaphore; // Declare the semaphore
    VkRenderPass renderPass; // Render pass
    VkExtent2D swapchainExtent; // Size of the swapchain images
    VkCommandBufferBeginInfo beginInfo{}; // Command buffer begin info
    VkAttachmentReference colorAttachmentRef{}; // Reference to the color attachment
    uint32_t currentImage = 0; // Index of the current image being rendered
};

#endif // VULKAN_RENDERER_H
