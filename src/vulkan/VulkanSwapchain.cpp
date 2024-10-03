#include "VulkanSwapchain.h"

VulkanSwapchain::VulkanSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkExtent2D windowExtent)
    : device(device), physicalDevice(physicalDevice), surface(surface), swapchainExtent(windowExtent) {}

VulkanSwapchain::~VulkanSwapchain() {
    cleanup();
}

void VulkanSwapchain::createImageViews() {
    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image views!");
        }
    }
}


void VulkanSwapchain::create() {
    // Query swap chain support details
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    // Choose the best surface format and present mode
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // Create the swapchain using the chosen parameters
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = swapChainSupport.capabilities.minImageCount + 1; // Double buffering
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Sharing mode (exclusive or concurrent)
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // No sharing between queues

    // Transform and composite alpha settings
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // Set the present mode
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    // Old swapchain (used during recreation)
    createInfo.oldSwapchain = VK_NULL_HANDLE; 

    // Create the swapchain
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain!");
    }

    // Retrieve the swapchain images
    uint32_t imageCount;  // Declare the variable to hold the number of images
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr); // Get the image count first

    swapchainImages.resize(imageCount); // Resize the vector to hold the images
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()); // Now get the images

    // Save the swapchain image format and extent for later use
    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    // Optionally: Create image views for the swapchain images here or in a separate function
    createImageViews(); // Call a method to create image views if needed
}

// Implementation of querySwapChainSupport in VulkanSwapchain
SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void VulkanSwapchain::createImageViews() {
    // Logic for creating image views for the swapchain images.
}

void VulkanSwapchain::cleanup() {
    for (VkImageView imageView : swapchainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    swapchainImageViews.clear();

    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }
}
