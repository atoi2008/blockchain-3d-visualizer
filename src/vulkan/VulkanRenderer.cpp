// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include <stdexcept>
#include <iostream>
#include <string.h>

VulkanRenderer::VulkanRenderer(GLFWwindow* win) : window(win) {} // Initialize window in constructor

void VulkanRenderer::init() {
    createInstance();
    // Create surface
    if (glfwCreateWindowSurface(instance, getWindow(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
    createSwapchain();
    createImageViews();
    createCommandBuffer(); // Call to create command buffer
}

void VulkanRenderer::cleanup() {
    cleanupSwapchain();
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer); // Free the command buffer
    vkDestroyCommandPool(device, commandPool, nullptr); // Destroy command pool
    vkDestroySurfaceKHR(instance, surface, nullptr); // Destroy surface
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool VulkanRenderer::isRunning() {
    return !glfwWindowShouldClose(window);
}

void VulkanRenderer::recreateSwapchain() {
    vkDeviceWaitIdle(device);
    cleanupSwapchain();
    createSwapchain();
    createImageViews();
    createCommandBuffer(); // Recreate command buffer
}

void VulkanRenderer::setFramebufferResizedFlag(bool resized) {
    framebufferResized = resized;
}

GLFWwindow* VulkanRenderer::getWindow() {
    return window; 
}

void VulkanRenderer::createInstance() {
    // Create Vulkan instance
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Additional app info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Blockchain 3D Visualizer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.pApplicationInfo = &appInfo;

    // Check for validation layers
    if (!checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    // Set validation layers
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
}

void VulkanRenderer::createSwapchain() {
    // Create swapchain logic
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface; 
    // Add additional parameters like minImageCount, imageFormat, etc.

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain!");
    }
}

void VulkanRenderer::createImageViews() {
    // Create image views from swapchain images
    // You may need to retrieve the images from the swapchain first
    // Then create VkImageView for each image
}

void VulkanRenderer::cleanupSwapchain() {
    for (auto imageView : swapchainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void VulkanRenderer::createCommandBuffer() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex; 
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }
}

void VulkanRenderer::render() {
    // Begin rendering command buffer
    VkResult result = vkBeginCommandBuffer(commandBuffer, nullptr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Specify rendering operations here (e.g., vkCmdBindPipeline(), vkCmdDraw(), etc.)

    // End recording command buffer
    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    // Submit command buffer to the graphics queue
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    // Present the swapchain image
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex; 

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}

bool VulkanRenderer::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Define the validation layers you want to check
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation" // Example validation layer
    };

    // Check if each requested validation layer is available
    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false; // Return false if any layer is not found
        }
    }

    return true; // All requested validation layers are supported
}
