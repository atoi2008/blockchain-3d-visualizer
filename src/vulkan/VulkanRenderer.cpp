#include "VulkanRenderer.h"
#include <stdexcept>
#include <iostream>
#include <string.h>

// This is a debug callback function is defined outside of the VulkanRenderer class.
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VulkanRenderer::VulkanRenderer(GLFWwindow* win) : window(win) {}

void VulkanRenderer::init() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createCommandBuffer();
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}


void VulkanRenderer::cleanup() {
    if (debugMessenger != VK_NULL_HANDLE) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    cleanupSwapchain(); // this needs to be done before destroying device
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
    createCommandBuffer();
}

void VulkanRenderer::setFramebufferResizedFlag(bool resized) {
    framebufferResized = resized;
}

void VulkanRenderer::checkFramebufferResized() {
    if (framebufferResized) {
        recreateSwapchain(); // This method should handle all the necessary steps
        framebufferResized = false; // Reset the flag
    }
}

GLFWwindow* VulkanRenderer::getWindow() {
    return window; 
}

void VulkanRenderer::createInstance() {
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Blockchain 3D Visualizer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.pApplicationInfo = &appInfo;

    // Check for validation layer support
    if (!checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    // Enable the debug utils extension
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Set up debug messenger creation info
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;

    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    // Create the debug messenger
    if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}


void VulkanRenderer::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

void VulkanRenderer::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Select the first available physical device for simplicity
    for (const auto& device : devices) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        if (indices.isComplete()) {
            physicalDevice = device;
            graphicsQueueFamilyIndex = indices.graphicsFamily.value();
            presentQueueFamilyIndex = indices.presentFamily.value();
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

void VulkanRenderer::createLogicalDevice() {
    // Create a logical device
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    // Graphics queue
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo graphicsQueueInfo{};
    graphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    graphicsQueueInfo.queueCount = 1; // Request one queue
    graphicsQueueInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(graphicsQueueInfo);

    // Only create a present queue if it's from a different queue family
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        VkDeviceQueueCreateInfo presentQueueInfo{};
        presentQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        presentQueueInfo.queueFamilyIndex = presentQueueFamilyIndex;
        presentQueueInfo.queueCount = 1; // Request one queue
        presentQueueInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(presentQueueInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE; // Enable anisotropic filtering

    // Specify the extensions to be used
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // Add other extensions if needed
    };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    // Enable the extensions
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    // Get handles to the graphics and present queues
    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
    } else {
        // If they are the same, use the graphics queue for presentation
        presentQueue = graphicsQueue;
    }
}
// swapchain helper functions
// Function to query swap chain support details for a physical device
SwapChainSupportDetails VulkanRenderer::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

// Function to choose the best surface format
VkSurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0]; // Return the first available format if preferred one is not found
}

// Function to choose the best present mode
VkPresentModeKHR VulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode; // Prefer mailbox mode if available
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR; // Fallback to FIFO mode
}

// Function to choose the swap extent
VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent; // Use current extent if valid
    }
    else
    {
        // Specify your desired width and height
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        // Clamp the extent to the min and max values
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void VulkanRenderer::createSwapchain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

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
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // No sharing between queues
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE; // This is for recreating

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain!");
    }

    // Retrieve the swapchain images (add this as needed)
    // vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    // vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
}


void VulkanRenderer::createImageViews() {
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

void VulkanRenderer::cleanupSwapchain() {
    for (VkImageView imageView : swapchainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    swapchainImageViews.clear();

    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE; // Set to null after destruction
    }
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

// Create Render Pass
void VulkanRenderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat; // Format of the swapchain images
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // Number of samples for the attachment
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear before rendering
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Store the results
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Not using stencil
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Not using stencil
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Final layout
    colorAttachmentRef.attachment = 0; // The index of the color attachment
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // The layout of the attachment


    // Create Render Pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;

    // Specify the subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef; // Set to the color attachment reference

    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // Create the render pass
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}


/*** Render Function */
void VulkanRenderer::render() {
    // Acquire the next image from the swapchain
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, nullptr, nullptr, &currentImage);

    // Ensure the command buffer is recorded
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer!");
    }

    // Define the render pass info
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass; // The render pass
    renderPassInfo.framebuffer = swapchainFramebuffers[currentImage]; // Framebuffer for current image
    renderPassInfo.renderArea.offset = {0, 0}; // Start rendering from the top-left corner
    renderPassInfo.renderArea.extent = swapchainExtent; // The extent of the framebuffer

    // Clear color
    VkClearValue clearColor = {};
    clearColor.color = {{1.0f, 1.0f, 1.0f, 1.0f}}; // White background
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    // Begin the render pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Rendering commands go here...

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer!");
    }
}




bool VulkanRenderer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}
