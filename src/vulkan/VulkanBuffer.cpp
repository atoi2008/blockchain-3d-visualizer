// VulkanBuffer.cpp

#include "VulkanBuffer.h"
#include <stdexcept>
#include <cstring>

VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, 
                           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : device(device), physicalDevice(physicalDevice)
{
    createBuffer(size, usage, properties);
    allocateMemory(size, properties);
}

VulkanBuffer::~VulkanBuffer() {
    cleanup();
}

void VulkanBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan buffer!");
    }
}

void VulkanBuffer::allocateMemory(VkDeviceSize size, VkMemoryPropertyFlags properties) {
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate Vulkan buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VulkanBuffer::copyDataToBuffer(void* data, VkDeviceSize size) {
    void* mappedData;
    vkMapMemory(device, bufferMemory, 0, size, 0, &mappedData);
    std::memcpy(mappedData, data, size);
    vkUnmapMemory(device, bufferMemory);
}

void VulkanBuffer::cleanup() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, buffer, nullptr);
    }
    if (bufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, bufferMemory, nullptr);
    }
}

// Helper function to find the appropriate memory type
uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
