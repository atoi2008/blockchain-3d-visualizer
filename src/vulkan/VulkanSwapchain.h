#pragma once

#include <vulkan/vulkan.h>
#include "VulkanRenderer.h"

class VulkanSwapchain {
public:
    VulkanSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkExtent2D windowExtent);
    ~VulkanSwapchain();

    void create();
    void cleanup();

    VkSwapchainKHR getSwapchain() const { return swapchain; }
    VkFormat getSwapchainImageFormat() const { return swapchainImageFormat; }
    VkExtent2D getSwapchainExtent() const { return swapchainExtent; }
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;

    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    void createImageViews();
};
