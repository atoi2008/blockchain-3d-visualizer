cmake_minimum_required(VERSION 3.10)

# Set project name
project(Blockchain3DVisualizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Check if vcpkg is used
if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Add the source directory
add_subdirectory(libs/randomx)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Find GLM
find_package(glm REQUIRED)

# Define include directories for RandomX
set(RANDOMX_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/libs/randomx/src")

# Include directories
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${RANDOMX_INCLUDE_DIRS})  # Include RandomX headers

# Add the executable and link libraries
add_executable(blockchain-3d-visualizer
    src/main.cpp
    src/mining/RandomXMiner.cpp
    src/vulkan/VulkanRenderer.cpp
)

# Link libraries
target_link_libraries(blockchain-3d-visualizer
    Vulkan::Vulkan
    glfw
    glm
    randomx 
)
