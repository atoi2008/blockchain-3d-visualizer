// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include <stdexcept>
#include <iostream>

void VulkanRenderer::init() {
    createInstance();
    createSwapchain();
    createImageViews();
    createCommandBuffer(); // Call to create command buffer
    // Additional Vulkan setup can be done here
}

void VulkanRenderer::cleanup() {
    cleanupSwapchain();
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer); // Free the command buffer
    vkDestroyCommandPool(device, commandPool, nullptr); // Destroy command pool
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool VulkanRenderer::isRunning() {
    return !glfwWindowShouldClose(window);  // Check if the window is still open
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
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = nullptr; // Add your application info if needed

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
}

void VulkanRenderer::createSwapchain() {
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface; // Set this to your created Vulkan surface
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
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex; // Replace with your queue family index
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
    VkResult result = vkBeginCommandBuffer(commandBuffer, nullptr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Specify rendering operations here (e.g., vkCmdBindPipeline(), vkCmdDraw(), etc.)

    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex; // Replace with your current image index

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}
