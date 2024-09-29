cmake_minimum_required(VERSION 3.10)

# Set project name
project(Blockchain3DVisualizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source directory
#add_subdirectory(src)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Add libraries
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/external/randomx)
include_directories(/usr/include/GLFW)  #TODO: Update path as needed
include_directories(/usr/include/glm)   #TODO: Update path as needed

# Add the executable and link libraries
add_executable(blockchain-3d-visualizer src/main.cpp src/mining/RandomXMiner.cpp src/vulkan/VulkanRenderer.cpp)

# Link libraries
target_link_libraries(blockchain-3d-visualizer Vulkan::Vulkan glfw)
