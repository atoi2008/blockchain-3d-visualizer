#include "VulkanRenderer.h"
#include <stdexcept>
#include <iostream>
#include <string.h>

// This is a debug callback function that is defined outside of the VulkanRenderer class.
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VulkanRenderer::VulkanRenderer(GLFWwindow* win) 
: window(win), device(VK_NULL_HANDLE), instance(VK_NULL_HANDLE) {}

void VulkanRenderer::init() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createCommandBuffer();
    createMiningUBOBuffer();
    createSyncObjects();
}

void VulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    // Buffer creation
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // If the buffer will only be used by one queue family

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }

    // Memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    // Allocate memory
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    // Bind the buffer with the memory
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VulkanRenderer::createMiningUBOBuffer() {
    miningUBOBuffer = new VulkanBuffer(device, physicalDevice, sizeof(MiningUBO), 
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VulkanRenderer::createSyncObjects() {
    // Semaphore creation code
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create semaphores!");
    }

    // Fence creation code
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence!");
        }
    }
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
    // Destroy debug messenger first
    if (debugMessenger != VK_NULL_HANDLE) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    
    // Destroy the semaphore
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    
    // Free command buffers before destroying the command pool
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    
    // Destroy command pool
    vkDestroyCommandPool(device, commandPool, nullptr);
    
    // Destroy the surface
    vkDestroySurfaceKHR(instance, surface, nullptr);
    
    // Clean up the swapchain before destroying the device
    cleanupSwapchain(); 
    
    // Destroy logical device
    vkDestroyDevice(device, nullptr);
    
    // Destroy Vulkan instance last
    vkDestroyInstance(instance, nullptr);
    
    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}


void VulkanRenderer::presentImage(uint32_t imageIndex) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphore; // Wait for rendering to complete
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }
}

/**
 * @brief Updates the mining-related data and visualizes it using Vulkan rendering.
 * 
 * This function workflow is responsible for updating the mining-related data and then rendering
 * the frame based on that data. It consists of three main stages:
 * 
 * 1. **update()**: Updates the Uniform Buffer Object (UBO) with the current mining data.
 *    - Takes a `BlockHeader` as input and extracts relevant mining information (like nonce, hash).
 *    - Maps this data into the UBO (uniform buffer) that will be used by the shaders.
 * 
 * 2. **updateUniformBuffer()**: Transfers updated data from the CPU to the GPU's memory.
 *    - Maps the memory of the uniform buffer, copies the UBO data, and unmaps it.
 *    - This ensures that the GPU has the latest mining data for rendering.
 * 
 * 3. **drawFrame()**: Manages synchronization, acquires the next image, submits drawing commands, and presents the image.
 *    - Waits for the GPU to finish rendering the previous frame.
 *    - Acquires the next image from the swapchain.
 *    - Submits the command buffer (including rendering commands) and presents the rendered image.
 *    - Handles synchronization between frames and manages GPU resources.
 * 
 * This structure allows for real-time visualization of the mining data using the Vulkan API, where mining progress or block data can be updated on the fly.
 */

void VulkanRenderer::update(const BlockHeader& blockHeader) {
    // Map block header data to the miningUBO (modify this as per your data needs)
    miningUBO.data[0] = static_cast<float>(blockHeader.nonce);  // Example
    miningUBO.data[1] = static_cast<float>(blockHeader.hash[0]); // Example for hash data
    
    // Update the uniform buffer on the GPU
    updateUniformBuffer();
}

void VulkanRenderer::updateUniformBuffer() {
    void* data;
    vkMapMemory(device, miningUBOBufferMemory, 0, sizeof(MiningUBO), 0, &data);
    memcpy(data, &miningUBO, sizeof(MiningUBO));
    vkUnmapMemory(device, miningUBOBufferMemory);
}

void VulkanRenderer::drawFrame() {
    // Wait for the previous frame to finish
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    //TODO: Submit the rendering command buffer for the acquired image
    // Submit commands here...

    // Present the image
    presentImage(imageIndex);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; // Cycle to the next frame
}



bool VulkanRenderer::isRunning() {
    return !glfwWindowShouldClose(window);
}

void VulkanRenderer::recreateSwapchain() {
    vkDeviceWaitIdle(device);

    // Wait for device to be idle before recreating swapchain
    vkDeviceWaitIdle(device);

    // Get new window size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    windowExtent.width = static_cast<uint32_t>(width);
    windowExtent.height = static_cast<uint32_t>(height);

    // Now recreate the swapchain with the updated windowExtent
    swapchain->recreate(windowExtent);

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

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    // Enable the debug utils extension
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
     // Create the swapchain using VulkanSwapchain
    swapchain = new VulkanSwapchain(device, physicalDevice, surface, windowExtent);
    swapchain->create();  // Call the encapsulated create method

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
    if (swapchain) {
        swapchain->cleanup();  // Call VulkanSwapchain's cleanup method
        delete swapchain;      // Clean up the swapchain object itself
        swapchain = nullptr;   // Set to nullptr after deletion
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
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &currentImage);

    // Update mining stats and the uniform buffer
    updateMiningStats();        // Update mining data with new values
    updateUniformBuffer();      // Copy updated mining data to the uniform buffer

    // Ensure the command buffer is recorded
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Set to zero for default behavior
    beginInfo.pInheritanceInfo = nullptr; // Only needed for secondary command buffers

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer!");
    }

    // Define the render pass info
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass; // Your render pass
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

    // ** Bind the UBO that contains mining data **
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                            0, 1, &descriptorSet, 0, nullptr); // Bind the descriptor set for UBO

    // Bind the graphics pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    // Issue drawing commands (procedural visualizations based on mining data)
    vkCmdDraw(commandBuffer, /*vertex count*/, /*instance count*/, /*first vertex*/, /*first instance*/);

    // End the render pass
    vkCmdEndRenderPass(commandBuffer);

    // End recording the command buffer
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer!");
    }

    // Submit and present the frame
    drawFrame();
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
