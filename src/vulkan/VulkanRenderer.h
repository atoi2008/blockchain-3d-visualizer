#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include "Block.h"
#include "VulkanBuffer.h"
#include "VulkanSwapchain.h"

struct MiningUBO
{
    float data[4]; // Example: store 4 floats for procedural generation (e.g., a nonce or hash)
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer
{
public:
    VulkanRenderer(GLFWwindow *win);
    void init();
    void cleanup();
    bool isRunning();
    void recreateSwapchain();
    void setFramebufferResizedFlag(bool resized);
    void checkFramebufferResized();
    void update(const BlockHeader &blockHeader);
    void render();
    GLFWwindow *getWindow();

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
    void presentImage(uint32_t imageIndex);
    void createMiningUBOBuffer();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void createSyncObjects();
    void drawFrame();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    GLFWwindow *window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VulkanSwapchain* swapchain;  // Use the encapsulated VulkanSwapchain class

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface; // Declare surface member variable

    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;

    bool framebufferResized = false;

    // Helper functions for swap chain
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    std::vector<VkImage> swapchainImages;             // To hold the swapchain images
    std::vector<VkImageView> swapchainImageViews;     // To hold the image views
    std::vector<VkFramebuffer> swapchainFramebuffers; // Framebuffers for the swapchain images
    VkClearValue clearColor;                          // Clear color value for the render pass
    VkFormat swapchainImageFormat;                    // To hold the format of the swapchain images
    VkSemaphore imageAvailableSemaphore;              // Declare the semaphore
    VkRenderPass renderPass;                          // Render pass
    VkExtent2D swapchainExtent;                       // Size of the swapchain images
    VkCommandBufferBeginInfo beginInfo{};             // Command buffer begin info
    VkAttachmentReference colorAttachmentRef{};       // Reference to the color attachment
    uint32_t currentImage = 0;                        // Index of the current image being rendered

    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    size_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;  // Adjust as needed
    std::vector<VkFence> inFlightFences; // To synchronize frame rendering

    // Add a UBO for the mining data
    MiningUBO miningUBO;
    VulkanBuffer* miningUBOBuffer;  // Pointer to VulkanBuffer instance
    VkDeviceMemory miningUBOBufferMemory;

    // Add a function to update the UBO on the GPU
    void updateUniformBuffer();
};

#endif // VULKAN_RENDERER_H
