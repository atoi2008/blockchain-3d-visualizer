// VulkanBuffer.h

#pragma once
#include <vulkan/vulkan.h>

class VulkanBuffer {
public:
    VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, 
                 VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~VulkanBuffer();

    VkBuffer getBuffer() const { return buffer; }
    VkDeviceMemory getMemory() const { return bufferMemory; }

    void copyDataToBuffer(void* data, VkDeviceSize size);  // Function to map and copy data
    void cleanup();

private:
    VkDevice device;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkPhysicalDevice physicalDevice;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void allocateMemory(VkDeviceSize size, VkMemoryPropertyFlags properties);
};
