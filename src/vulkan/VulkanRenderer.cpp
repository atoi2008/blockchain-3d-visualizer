// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include <stdexcept>
#include <iostream>

void VulkanRenderer::init() {
    createInstance();
    // Additional Vulkan setup
}

void VulkanRenderer::render() {
    // Render logic here
}

void VulkanRenderer::cleanup() {
    cleanupSwapchain();
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
    createFramebuffers();
}

void VulkanRenderer::setFramebufferResizedFlag(bool resized) {
    framebufferResized = resized;
}

GLFWwindow* VulkanRenderer::getWindow() {
    return window;
}

// Additional Vulkan creation methods (createInstance, createSwapchain, etc.)...
