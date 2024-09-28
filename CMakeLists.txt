cmake_minimum_required(VERSION 3.10)
project(Blockchain3DVisualizer)

set(CMAKE_CXX_STANDARD 17)

# Find Vulkan and GLFW
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})

# Add the executable and source files
add_executable(Blockchain3DVisualizer 
    src/main.cpp 
    src/vulkan/VulkanRenderer.cpp 
    src/mining/RandomXMiner.cpp
)

# Link Vulkan and GLFW libraries
target_link_libraries(Blockchain3DVisualizer ${Vulkan_LIBRARIES} glfw)
