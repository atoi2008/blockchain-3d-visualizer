// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include <iostream>
#include <stdexcept>

VulkanRenderer::VulkanRenderer() 
    : framebufferResized(false), window(nullptr), device(VK_NULL_HANDLE), swapChain(VK_NULL_HANDLE) {}

void VulkanRenderer::init() {
    try {
        createInstance();
        createDevice(); // Make sure to add device creation here
        createSwapchain();
        createImageViews();
        createFramebuffers();
        createCommandBuffers();
        std::cout << "Vulkan renderer initialized." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to initialize Vulkan renderer: " << e.what() << std::endl;
        throw; // Rethrow to let main handle the cleanup
    }
}

void VulkanRenderer::recreateSwapchain() {
    vkDeviceWaitIdle(device); // Ensure the device is idle before making changes
    cleanupSwapchain();

    // Create a new swapchain and related resources
    createSwapchain();
    createImageViews();
    createFramebuffers();
    createCommandBuffers();
}

void VulkanRenderer::cleanupSwapchain() {
    // Clean up swapchain-related resources
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }
}

void VulkanRenderer::setFramebufferResizedFlag(bool resized) {
    framebufferResized = resized;
}

GLFWwindow* VulkanRenderer::getWindow() {
    return window;
}

bool VulkanRenderer::isRunning() {
    return !glfwWindowShouldClose(window);
}

void VulkanRenderer::render() {
    // Placeholder for rendering code
    std::cout << "Rendering frame..." << std::endl;

    // Example of waiting on the swap chain and rendering (this needs to be expanded)
    // vkWaitForFences(...);
    // vkAcquireNextImageKHR(...);
    // vkQueueSubmit(...);
}

void VulkanRenderer::cleanup() {
    cleanupSwapchain();
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
    }
    // Additional cleanup, such as destroying the instance
    std::cout << "Vulkan renderer cleaned up." << std::endl;
}
