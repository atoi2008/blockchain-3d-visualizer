// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include <iostream>

VulkanRenderer::VulkanRenderer() : framebufferResized(false) {}

void VulkanRenderer::init() {
    // Initialize Vulkan (setup instance, device, swapchain, etc.)
    createInstance();
    createSwapchain();
    createImageViews();
    createFramebuffers();
    createCommandBuffers();
    std::cout << "Vulkan renderer initialized." << std::endl;
}

void VulkanRenderer::recreateSwapchain() {
    // Wait for the device to be idle before modifying Vulkan resources
    vkDeviceWaitIdle(device);

    // Destroy old swapchain-related resources
    cleanupSwapchain();

    // Create a new swapchain with updated dimensions
    createSwapchain();
    createImageViews();
    createFramebuffers();
    createCommandBuffers();
}

void VulkanRenderer::cleanupSwapchain() {
    // Destroy framebuffers, image views, etc.
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    // Destroy the swapchain itself
    vkDestroySwapchainKHR(device, swapChain, nullptr);
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
    // Example render function (replace with actual rendering code)
    std::cout << "Rendering frame..." << std::endl;
}

void VulkanRenderer::cleanup() {
    // Cleanup Vulkan resources (swapchain, instance, etc.)
    cleanupSwapchain();
    std::cout << "Vulkan renderer cleaned up." << std::endl;
}
